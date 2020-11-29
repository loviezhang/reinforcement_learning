#include <stdio.h>
#include <array>
#include <random>

class arm {
public:
    arm(double mean, double stddev)
        : mean_(mean), stddev_(stddev) {
    }

    double bandit() {
        std::random_device rd{};
        std::mt19937 gen{rd()};
        std::normal_distribution<> d{mean_, stddev_};
        return d(gen);
    }

private:
    double mean_;
    double stddev_;
};

template<int arm_num>
class armed_testbed {
public:
    armed_testbed(double epsilion) {
        epsilion_ = epsilion;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-2.0, 2.0);
        // 初始化臂
        for (int i = 0; i < arm_num; i++) {
            arms_.emplace_back(dis(gen), 1);
        }
        // 评估值初始化为0
        for (int i = 0; i < arm_num; i++) {
            times_[i] = 0;
            estimate_[i] = 0;
        }
    }

    // 摇臂，并得到收益
    double wave() {
        // 选择摇哪个
        int i = 0;
        if (greedy()) {
            i = arm_greedy();
        } else {
            i = arm_random();
        }

        // 拿到收益
        double reward = arms_[i].bandit();

        printf("num: %d, reward: %.4f\n", i, reward);

        // 更新
        update_reward(i, reward);

        return reward;
    }

private:
    // 随机探测还是贪心
    bool greedy() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        return dis(gen) > epsilion_;
    }

    // 随机选择一个臂
    int arm_random() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, arm_num-1);
        return dis(gen);
    }

    // 按照贪心的策略选择一个臂
    int arm_greedy() {
        int i_max = 0;
        double max = estimate_[0];
        for (int i = 1; i < arm_num; i++) {
            if (estimate_[i] > max) {
                max = estimate_[i];
                i_max = i;
            }
        }
        return i_max;
    }

    void update_reward(int i, double reward) {
        if (times_[i] == 0) {
            estimate_[i] = reward;
        } else {
            estimate_[i] = estimate_[i] + (reward - estimate_[i]) / (double)times_[i];
        }
        times_[i]++;
    }

private:
    std::vector<arm> arms_;
    std::array<int, arm_num> times_;
    std::array<double, arm_num> estimate_;

    double epsilion_;
};

int main(int argc, char** argv) {
    armed_testbed<10> at(0.1);
    for (int i = 0; i < 100; i++) {
        at.wave();
    }
    return 0;
}
