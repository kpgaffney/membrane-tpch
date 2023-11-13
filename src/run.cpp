#include <bit>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include <duckdb.hpp>
#include <ittnotify.h>
#include <oneapi/tbb.h>

int num_trials = 6;

template <typename F> float time(F &&f) {
  auto t0 = std::chrono::steady_clock::now();
  f();
  auto t1 = std::chrono::steady_clock::now();
  return std::chrono::duration<float>(t1 - t0).count();
}

std::string read(const std::string &filename) {
  std::ifstream file(filename);

  if (!file.is_open()) {
    throw std::logic_error("unable to open file");
  }

  return {(std::istreambuf_iterator<char>(file)),
          std::istreambuf_iterator<char>()};
}

struct Query {
  void evaluate(duckdb::Connection &con, std::ofstream &out) {
    std::string sql_standard = read("sql/" + name + "-standard.sql");
    std::string sql_prejoined = read("sql/" + name + "-prejoined.sql");
    std::string sql_selection = read("sql/" + name + "-selection.sql");
    std::string sql_projection = read("sql/" + name + "-projection.sql");
    std::string sql_selproj = read("sql/" + name + "-selproj.sql");
    std::string sql_aggregate = read("sql/" + name + "-aggregate.sql");

    for (int trial = 0; trial < num_trials; ++trial) {
      std::string domain_name_standard = name + "-standard";
      ___itt_domain *domain_standard =
          __itt_domain_create(domain_name_standard.c_str());
      __itt_resume();
      __itt_frame_begin_v3(domain_standard, nullptr);
      float time_standard = time([&] { con.Query(sql_standard)->Print(); });
      __itt_frame_end_v3(domain_standard, nullptr);
      __itt_pause();
      out << name << ',' << trial << ",standard," << time_standard << '\n';

      std::string domain_name_prejoined = name + "-prejoined";
      ___itt_domain *domain_prejoined =
          __itt_domain_create(domain_name_prejoined.c_str());
      __itt_resume();
      __itt_frame_begin_v3(domain_prejoined, nullptr);
      float time_prejoined = time([&] { con.Query(sql_prejoined)->Print(); });
      __itt_frame_end_v3(domain_prejoined, nullptr);
      __itt_pause();
      out << name << ',' << trial << ",prejoined," << time_prejoined << '\n';

      con.Query(sql_selproj)->Print();

      float time_aggregate = time([&] { con.Query(sql_aggregate)->Print(); });
      out << name << ',' << trial << ",aggregate," << time_aggregate << '\n';

      con.Query("DROP TABLE selproj")->Print();

      auto n_src =
          con.Query("SELECT count(*) FROM lineitem")->GetValue<size_t>(0, 0);
      size_t n_dst = 0;

      std::vector<uint64_t> bitmap(n_src / 64 + (n_src % 64 != 0));
      std::unique_ptr<duckdb::QueryResult> selection = con.Query(sql_selection);
      for (auto row : *selection) {
        auto rowid = row.GetValue<uint64_t>(0);
        bitmap[rowid / 64] |= uint64_t(1) << rowid % 64;
        ++n_dst;
      }

      float time_gather =
          gather(n_src, n_dst, con.Query(sql_projection), bitmap);
      out << name << ',' << trial << ",gather," << time_gather << '\n';
    }
  }

  virtual float gather(size_t n_src,
                       size_t n_dst,
                       std::unique_ptr<duckdb::QueryResult> projection,
                       const std::vector<uint64_t> &bitmap) = 0;

  std::string name;
};

template <typename T>
float gather_one(size_t n_src,
                 size_t n_dst,
                 const std::unique_ptr<duckdb::QueryResult> &projection,
                 const std::vector<uint64_t> &bitmap,
                 size_t col) {
  std::vector<T> src(n_src);
  std::vector<T> dst(n_dst);

  for (auto row : *projection) {
    src.push_back(row.GetValue<T>(col));
  }

  return time([&] {
    std::atomic_size_t cursor(0);

    tbb::parallel_for(tbb::blocked_range<size_t>(0, bitmap.size()),
                      [&](const tbb::blocked_range<size_t> &r) {
                        size_t count = 0;
                        for (size_t i = r.begin(); i < r.end(); ++i) {
                          count += std::popcount(bitmap[i]);
                        }

                        if (count != 0) {
                          size_t index = cursor.fetch_add(count);
                          for (size_t i = r.begin(); i < r.end(); ++i) {
                            uint64_t j = i * 64;
                            uint64_t m = bitmap[i];
                            while (m != 0) {
                              size_t k = std::countr_zero(m);
                              dst[index++] = std::move(src[j + k]);
                              m ^= (uint64_t(1) << k);
                            }
                          }
                        }
                      });
  });
}

struct Query03 : public Query {
  Query03() { name = "q03"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<int32_t>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 1);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 2);
    time_gather += gather_one<int32_t>(n_src, n_dst, projection, bitmap, 3);
    time_gather += gather_one<int32_t>(n_src, n_dst, projection, bitmap, 4);
    return time_gather;
  }
};

struct Query05 : public Query {
  Query05() { name = "q05"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 1);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 2);
    return time_gather;
  }
};

struct Query06 : public Query {
  Query06() { name = "q06"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 1);
    return time_gather;
  }
};

struct Query08 : public Query {
  Query08() { name = "q08"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<uint16_t>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 1);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 2);
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 3);
    return time_gather;
  }
};

struct Query10 : public Query {
  Query10() { name = "q10"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<int32_t>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 1);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 2);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 3);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 4);
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 5);
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 6);
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 7);
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 8);
    return time_gather;
  }
};

struct Query12 : public Query {
  Query12() { name = "q12"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 1);
    return time_gather;
  }
};

struct Query14 : public Query {
  Query14() { name = "q14"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<std::string>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 1);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 2);
    return time_gather;
  }
};

struct Query19 : public Query {
  Query19() { name = "q19"; }

  float gather(size_t n_src,
               size_t n_dst,
               std::unique_ptr<duckdb::QueryResult> projection,
               const std::vector<uint64_t> &bitmap) override {
    float time_gather = 0;
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 0);
    time_gather += gather_one<float>(n_src, n_dst, projection, bitmap, 1);
    return time_gather;
  }
};

int main(int argc, char **argv) {
  if (argc != 2) {
    throw std::logic_error("usage: run DB_PATH");
  }

  duckdb::DuckDB db(argv[1]);
  duckdb::Connection con(db);

  std::ofstream out("results.csv");
  if (!out.is_open()) {
    throw std::logic_error("unable to open file");
  }

  Query03().evaluate(con, out);
  Query05().evaluate(con, out);
  Query06().evaluate(con, out);
  Query08().evaluate(con, out);
  Query10().evaluate(con, out);
  Query12().evaluate(con, out);
  Query14().evaluate(con, out);
  Query19().evaluate(con, out);

  return 0;
}
