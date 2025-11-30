#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>

using namespace std;

// --- 1. CẤU HÌNH BÀI TOÁN ---
const int DIM = 2;              // Số chiều (VD: x, y)
const int N_PARTICLES = 30;     // Số lượng hạt
const int N_ITERATIONS = 50;    // Số vòng lặp
const double W = 0.5;           // Trọng số quán tính
const double C1 = 1.5;          // Hệ số nhận thức (Cá nhân)
const double C2 = 1.5;          // Hệ số xã hội (Bầy đàn)
const double MIN_X = -10.0;     // Giới hạn dưới
const double MAX_X = 10.0;      // Giới hạn trên

// --- 2. HÀM MỤC TIÊU (Objective Function) ---
// Hàm Sphere: f(x) = x1^2 + x2^2 + ... (ở đây cho R^2)
double fitness_function(const vector<double>& position) {
    double sum = 0.0;
    for (double x : position) {
        sum += x * x;
    }
    return sum;
}

// Hàm sinh số ngẫu nhiên trong khoảng [min, max]
double random_double(double min, double max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

// --- 3. ĐỊNH NGHĨA HẠT (Particle) ---
struct Particle {
    vector<double> position;
    vector<double> velocity;
    vector<double> pbest_position;
    double pbest_value;
    double current_fitness;

    // Constructer
    Particle() {
        position.resize(DIM);
        velocity.resize(DIM);
        pbest_position.resize(DIM);
        
        // Khởi tạo vị trí và vận tốc ngẫu nhiên
        for (int i = 0; i < DIM; ++i) {
            position[i] = random_double(MIN_X, MAX_X);
            velocity[i] = random_double(-1.0, 1.0);
            pbest_position[i] = position[i];
        }
        pbest_value = numeric_limits<double>::infinity();
    }
};

// --- 4. CHƯƠNG TRÌNH CHÍNH ---
int main() {
    // Khởi tạo bầy đàn
    vector<Particle> swarm(N_PARTICLES);
    
    // Khởi tạo Gbest (Global Best)
    vector<double> gbest_position(DIM);
    double gbest_value = numeric_limits<double>::infinity();

    cout << "--- BAT DAU THUAT TOAN PSO ---" << endl;

    // Vòng lặp tối ưu hóa
    for (int iter = 0; iter < N_ITERATIONS; ++iter) {
        
        for (int i = 0; i < N_PARTICLES; ++i) {
            // 1. Đánh giá độ thích nghi (Fitness)
            swarm[i].current_fitness = fitness_function(swarm[i].position);

            // 2. Cập nhật Pbest (Kinh nghiệm cá nhân)
            if (swarm[i].current_fitness < swarm[i].pbest_value) {
                swarm[i].pbest_value = swarm[i].current_fitness;
                swarm[i].pbest_position = swarm[i].position;
            }

            // 3. Cập nhật Gbest (Kinh nghiệm bầy đàn)
            if (swarm[i].current_fitness < gbest_value) {
                gbest_value = swarm[i].current_fitness;
                gbest_position = swarm[i].position;
            }
        }

        // 4. Cập nhật Vận tốc và Vị trí cho từng hạt
        for (int i = 0; i < N_PARTICLES; ++i) {
            for (int d = 0; d < DIM; ++d) {
                double r1 = random_double(0.0, 1.0);
                double r2 = random_double(0.0, 1.0);

                // Công thức cập nhật vận tốc:
                // v = w * v + c1 * r1 * (pbest - x) + c2 * r2 * (gbest - x)
                swarm[i].velocity[d] = 
                    (W * swarm[i].velocity[d]) +
                    (C1 * r1 * (swarm[i].pbest_position[d] - swarm[i].position[d])) +
                    (C2 * r2 * (gbest_position[d] - swarm[i].position[d]));

                // Công thức cập nhật vị trí: x = x + v
                swarm[i].position[d] += swarm[i].velocity[d];

                // (Optional) Giới hạn không gian tìm kiếm
                if (swarm[i].position[d] < MIN_X) swarm[i].position[d] = MIN_X;
                if (swarm[i].position[d] > MAX_X) swarm[i].position[d] = MAX_X;
            }
        }

        // In kết quả mỗi vòng lặp
            cout << "Vong lap " << iter + 1 << ": Gbest = " << gbest_value << endl;
        
    }

    cout << "\n--- KET QUA CUOI CUNG ---" << endl;
    cout << "Vi tri toi uu tim duoc: [ ";
    for (double x : gbest_position) {
        cout << x << " ";
    }
    cout << "]" << endl;
    cout << "Gia tri nho nhat (Cost): " << gbest_value << endl;
}