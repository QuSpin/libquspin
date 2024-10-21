

#include <concepts>
#include <thread>
#include <vector>

namespace quspin {
  namespace details {

    template <typename Type>
    concept WorkTasksInterface = requires(Type work_tasks, const std::size_t task_id) {
      { work_tasks.size() } -> std::convertible_to<std::size_t>;
      { work_tasks.do_work(task_id) } -> std::same_as<void>;
    };

    enum class Schedule { Interleaved, SequentialBlocks };

    template <typename WorkTasks>
      requires WorkTasksInterface<WorkTasks>
    class WorkQueue {
      WorkTasks work_tasks;

    public:
      WorkQueue(WorkTasks work_tasks) : work_tasks(work_tasks) {}

      std::tuple<std::size_t, std::size_t, std::size_t> get_schedule(
          const Schedule schedule, const std::size_t nt, const std::size_t num_threads) {
        switch (schedule) {
          case Schedule::Interleaved: {
            return {nt, work_tasks.size(), num_threads};
          }
          case Schedule::SequentialBlocks: {
            const std::size_t num_jobs = work_tasks.size();
            const std::size_t chunk_size = (num_jobs + num_threads) / num_threads;
            const std::size_t start = nt * chunk_size;
            const std::size_t end = std::min(num_jobs, (nt + 1) * chunk_size);
            return {start, end, 1};
          }
        }
      }

      void run(const Schedule schedule, const std::size_t num_threads) {
        if (num_threads <= 0) {
          for (std::size_t i = 0; i < work_tasks.size(); i++) {
            work_tasks.do_work(i);
          }
          return;
        }

        std::vector<std::thread> workers;
        for (std::size_t nt = 0; nt < num_threads; nt++) {
          auto res = this->get_schedule(schedule, nt, num_threads);
          const std::size_t start = std::get<0>(res);
          const std::size_t end = std::get<1>(res);
          const std::size_t step = std::get<2>(res);
          workers.emplace_back(std::thread([res, this]() {
            for (std::size_t i = start; i < end; i += step) {
              this->work_tasks.do_work(i);
            }
          }));
        }

        for (auto &f : workers) {
          f.join();
        }
      }

      void run_dynamic(const std::size_t num_threads, const std::size_t chunk_size) {
        if (num_threads <= 0) {
          for (std::size_t i = 0; i < work_tasks.size(); i++) {
            work_tasks.do_work(i);
          }
          return;
        }

        std::size_t processed = 0;
        const std::size_t num_tasks = work_tasks.size();
        while (processed < num_tasks) {
          std::vector<std::thread> workers;
          for (std::size_t nt = 0; nt < num_threads; nt++) {
            const std::size_t start = processed;
            const std::size_t end = std::min(num_tasks, processed + chunk_size);
            if (start >= end) {
              break;
            }

            workers.emplace_back(std::thread([start, end, this]() {
              for (std::size_t i = start; i < end; i++) {
                this->work_tasks.do_work(i);
              }
            }));
            processed = end;
          }
          // wait for all threads to finish
          for (auto &f : workers) {
            f.join();
          }
        }
      }
    };

  }  // namespace details
}  // namespace quspin
