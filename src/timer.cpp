#include <functional>
#include <thread>

class Timer {
private:
  bool clear;
  int interval;
  std::function<void()> tick;

public:
  Timer(std::function<void()> tick, int interval)
      : tick(tick), interval(interval) {}

  void start() {
    this->clear = false;

    while (true) {
      if (this->clear)
        return;
      std::this_thread::sleep_for(std::chrono::milliseconds(interval));
      if (this->clear)
        return;
      tick();
    }
  }

  void stop() { this->clear = true; }
};
