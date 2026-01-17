#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// --- Cấu hình Bài toán (Knapsack) ---
const int NUM_ITEMS = 10;
const int CAPACITY = 50;
int weights[NUM_ITEMS] = {10, 20, 30, 15, 5, 8, 12, 6, 25, 4};
int values[NUM_ITEMS]  = {60, 100, 120, 70, 30, 45, 50, 20, 90, 15};

// --- Tham số PSO ---
const int SWARM_SIZE = 30;  // Số lượng hạt
const int MAX_ITER = 100;   // Số vòng lặp tối đa
const double W = 0.5;       // Quán tính
const double C1 = 1.5;      // Hệ số cá nhân
const double C2 = 1.5;      // Hệ số xã hội

// Cấu trúc một Hạt (Particle)
struct Particle {
    vector<int> position;       // Vị trí: 0 hoặc 1 (Nhị phân)
    vector<double> velocity;    // Vận tốc: Số thực
    vector<int> pbest_pos;      // Vị trí tốt nhất cá nhân
    double pbest_fitness;       // Giá trị tốt nhất cá nhân
    double current_fitness;     // Giá trị hiện tại

    Particle() {
        position.resize(NUM_ITEMS);
        velocity.resize(NUM_ITEMS);
        pbest_pos.resize(NUM_ITEMS);
    }
};

// Hàm Sigmoid: Chuyển vận tốc thành xác suất 
double sigmoid(double v) {
    return 1.0 / (1.0 + exp(-v));
}

// Hàm đánh giá (Fitness Function)
double evaluate(const vector<int>& pos) {
    int total_weight = 0;
    int total_value = 0;

    for (int i = 0; i < NUM_ITEMS; ++i) {
        if (pos[i] == 1) {
            total_weight += weights[i];
            total_value += values[i];
        }
    }

    // Nếu vượt quá sức chứa, trả về 0 (phạt nặng)
    if (total_weight > CAPACITY) {
        return 0;
    }
    return (double)total_value;
}

int main() {
    srand(time(0));

    vector<Particle> swarm(SWARM_SIZE);
    vector<int> gbest_pos(NUM_ITEMS);
    double gbest_fitness = -1.0;

    // 1. Khởi tạo đàn hạt
    for (int i = 0; i < SWARM_SIZE; ++i) {
        for (int j = 0; j < NUM_ITEMS; ++j) {
            // Khởi tạo vị trí ngẫu nhiên 0 hoặc 1
            swarm[i].position[j] = rand() % 2;
            // Khởi tạo vận tốc ngẫu nhiên nhỏ
            swarm[i].velocity[j] = ((double)rand() / RAND_MAX) * 2 - 1; 
        }
        
        swarm[i].current_fitness = evaluate(swarm[i].position);
        swarm[i].pbest_pos = swarm[i].position;
        swarm[i].pbest_fitness = swarm[i].current_fitness;

        // Cập nhật Gbest ban đầu
        if (swarm[i].pbest_fitness > gbest_fitness) {
            gbest_fitness = swarm[i].pbest_fitness;
            gbest_pos = swarm[i].pbest_pos;
        }
    }

    // 2. Vòng lặp chính
    for (int iter = 0; iter < MAX_ITER; ++iter) {
        for (int i = 0; i < SWARM_SIZE; ++i) {
            for (int d = 0; d < NUM_ITEMS; ++d) {
                // A. Cập nhật vận tốc (như công thức gốc) [cite: 251]
                double r1 = (double)rand() / RAND_MAX;
                double r2 = (double)rand() / RAND_MAX;
                
                swarm[i].velocity[d] = W * swarm[i].velocity[d]
                                     + C1 * r1 * (swarm[i].pbest_pos[d] - swarm[i].position[d])
                                     + C2 * r2 * (gbest_pos[d] - swarm[i].position[d]);

                // B. Cập nhật vị trí dùng Sigmoid 
                // Nếu rand() < S(v) -> bit = 1, ngược lại bit = 0
                double sig = sigmoid(swarm[i].velocity[d]);
                if ((double)rand() / RAND_MAX < sig) {
                    swarm[i].position[d] = 1;
                } else {
                    swarm[i].position[d] = 0;
                }
            }

            // C. Đánh giá lại hạt
            swarm[i].current_fitness = evaluate(swarm[i].position);

            // Cập nhật Pbest
            if (swarm[i].current_fitness > swarm[i].pbest_fitness) {
                swarm[i].pbest_fitness = swarm[i].current_fitness;
                swarm[i].pbest_pos = swarm[i].position;
            }
        }
        
        // Cập nhật Gbest sau mỗi vòng lặp (Synchronous update)
        for (int i = 0; i < SWARM_SIZE; ++i) {
             if (swarm[i].pbest_fitness > gbest_fitness) {
                gbest_fitness = swarm[i].pbest_fitness;
                gbest_pos = swarm[i].pbest_pos;
            }
        }
        cout << "Iter " << iter + 1 << ": Best Value = " << gbest_fitness << endl;
    }

    // 3. Kết quả cuối cùng
    cout << "=== KET QUA BPSO CHO BAI TOAN CAI TUI ===" << endl;
    cout << "Tong gia tri lon nhat tim duoc: " << gbest_fitness << endl;
    cout << "Cac vat duoc chon (1: Chon, 0: Khong): ";
    for (int val : gbest_pos) cout << val << " ";
    cout << endl;
}
