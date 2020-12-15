#include <iostream>
#include <list>
#include <deque>
#include <future>
#include <mutex>
#include <condition_variable>

// to avoid interleaved outputs
static std::mutex output_guard;

// synchro stuff
static std::deque<std::future<int>> task_queue;
static std::mutex task_guard;
static std::condition_variable task_signal;
static std::atomic_int pending_task_count = 0;

static int node_function(int task_id)
{
    {// TRACE
        const std::lock_guard<std::mutex> lock(output_guard);
        std::cout << "\t\t\t\t\t[" << std::this_thread::get_id() << "]";
        std::cout << "\tSTART task " << task_id << std::endl;
    }
    // emit some recursive tasks
    if (task_id < 5 || (task_id > 99 && task_id < 105))
    {
        for (auto n = 10; --n;)
        {
            auto k = task_id * 10 + n;
            auto subtask = std::async(std::launch::deferred, node_function, k);
            {// TRACE
                const std::lock_guard<std::mutex> lock(output_guard);
                std::cout << "\t\t\t\t\t[" << std::this_thread::get_id() << "]";
                std::cout << "\tSPAWN task " << k << std::endl;
            }
            {
                const std::lock_guard<std::mutex> lock(task_guard);
                task_queue.emplace_back(std::move(subtask));
                ++pending_task_count;
            }
            task_signal.notify_one(); // be sure at least one unit is awaken

            // modify delay from 1 to 100 to see idle behaviour
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    // DUMMY WORK
    std::this_thread::sleep_for(std::chrono::milliseconds((task_id % 3) * 100));

    // terminate task
    if (!--pending_task_count)
    {
        {// TRACE
            std::cout << "\t\t\t\t\t[" << std::this_thread::get_id() << "]";
            std::cout << "\tEMIT SHUTDOWN SIGNAL " << std::endl;
        }
        // awake all idle units for auto-shutdown
        task_signal.notify_all();
    }
    {// TRACE
        const std::lock_guard<std::mutex> lock(output_guard);
        std::cout << "\t\t\t\t\t[" << std::this_thread::get_id() << "]";
        std::cout << "\tCOMPLETE task " << task_id << std::endl;
    }
    return task_id;
};

int main()
{
    std::cout << "START system [" << std::this_thread::get_id() << "]" << std::endl;

    // emit initial task
    auto task = std::async(std::launch::deferred, node_function, 0);
    {// TRACE
        std::cout << "\t\t\t\t\t[" << std::this_thread::get_id() << "]";
        std::cout << "\tSPAWN task " << 0 << std::endl;
    }
    {
        const std::lock_guard<std::mutex> lock(task_guard);
        task_queue.emplace_back(std::move(task));
        ++pending_task_count;
    }

    // launch processing units
    std::list<std::thread> units;
    for (auto n = std::thread::hardware_concurrency(); --n;)
    {
        units.emplace_back(std::thread([n]()
            {
                {// TRACE
                    const std::lock_guard<std::mutex> lock(output_guard);
                    std::cout << "Unit #" << n;
                    std::cout << "\tLAUNCH [" << std::this_thread::get_id() << "]" << std::endl;
                }
                std::list<int> completed_tasks;
                {
                    std::unique_lock<std::mutex> lock(task_guard);
                    while (true)
                    {
                        if (!task_queue.empty())
                        {
                            // pick a task
                            auto task = std::move(task_queue.front());
                            task_queue.pop_front();
                            lock.unlock(); // unlock while processing task
                            {
                                // process task
                                int i = task.get();
                                completed_tasks.push_back(i);
                            }
                            lock.lock(); // reacquire lock
                        }
                        else if (!pending_task_count)
                            break;
                        else
                        {
                            {// TRACE
                                const std::lock_guard<std::mutex> lock(output_guard);
                                std::cout << "Unit #" << n;
                                std::cout << "\tPAUSE" << std::endl;
                            }
                            // IDLE
                            task_signal.wait(lock);
                            {// TRACE
                                const std::lock_guard<std::mutex> lock(output_guard);
                                std::cout << "Unit #" << n;
                                std::cout << "\tAWAKE" << std::endl;
                            }
                        }
                    }
                }
                {// TRACE
                    const std::lock_guard<std::mutex> lock(output_guard);
                    std::cout << "Unit #" << n;
                    std::cout << "\tTERMINATE {";
                    for (auto i : completed_tasks)
                    {
                        std::cout << " " << i << " ";
                    }
                    std::cout << "}" << std::endl;
                }
            }));
    }

    // wait for shutdown
    for (auto& unit : units)
    {
        unit.join();
    }
}
