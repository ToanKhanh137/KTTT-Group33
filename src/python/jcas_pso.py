import numpy as np
import matplotlib.pyplot as plt

# ==========================================
# 1. CẤU HÌNH HỆ THỐNG
# ==========================================
M = 16              # Số lượng anten phát (ULA)
K = 2               # Số lượng người dùng (Users)
L = 1               # Số lượng mục tiêu Radar (Targets)
P_max = 1.0         # Tổng công suất phát tối đa (Watts)
Noise_power = 0.01  # Công suất nhiễu

# Góc (độ)
theta_users = np.array([-30, 10]) * np.pi / 180  # Vị trí người dùng
theta_target = np.array([40]) * np.pi / 180      # Vị trí mục tiêu radar

# Tham số PSO
n_particles = 40    # Số lượng hạt
n_iterations = 200  # Số vòng lặp

# ==========================================
# 2. CÁC HÀM HỖ TRỢ (SYSTEM MODEL)
# ==========================================
def steering_vector(theta, M):
    """Tạo vector lái (Steering Vector) cho ULA"""
    d_lambda = 0.5
    idx = np.arange(M).reshape(-1, 1)
    return np.exp(1j * 2 * np.pi * d_lambda * idx * np.sin(theta))

# Tạo kênh truyền (Channel) giả lập cho người dùng (Line-of-Sight đơn giản)
H = np.zeros((M, K), dtype=complex)
for k in range(K):
    H[:, [k]] = steering_vector(theta_users[k], M)

# Tạo vector lái cho mục tiêu Radar
A_target = steering_vector(theta_target[0], M)

# ==========================================
# 3. HÀM MỤC TIÊU (FITNESS FUNCTION)
# ==========================================
def calculate_fitness(W_flat):
    # Giải mã vector thực thành ma trận phức W (M x (K+1))
    # K cột đầu cho Users, cột cuối cho Radar
    W_complex = W_flat[:M*(K+1)] + 1j * W_flat[M*(K+1):]
    W = W_complex.reshape(M, K+1)
    
    # 1. Tính Sum Rate (Truyền thông)
    W_users = W[:, :K]
    SINR_sum = 0
    for k in range(K):
        h_k = H[:, k]
        w_k = W_users[:, k]
        signal = np.abs(np.vdot(h_k, w_k))**2
        interference = 0
        for j in range(K):
            if j != k:
                interference += np.abs(np.vdot(h_k, W_users[:, j]))**2
        SINR_k = signal / (interference + Noise_power)
        SINR_sum += np.log2(1 + SINR_k)
    
    # 2. Tính Sensing Gain (Radar)
    # Tổng hợp tất cả các chùm tia
    # Gain tại hướng mục tiêu
    total_response = np.abs(np.vdot(A_target, np.sum(W, axis=1)))**2
    
    # Hàm mục tiêu: Kết hợp Rate và Sensing Gain
    # Trọng số alpha=0.5, beta=0.5 (có thể chỉnh)
    fitness = 0.6 * SINR_sum + 0.4 * np.log10(1 + total_response)
    return fitness

def check_power_constraint(W_flat):
    W_complex = W_flat[:M*(K+1)] + 1j * W_flat[M*(K+1):]
    current_power = np.sum(np.abs(W_complex)**2)
    if current_power > P_max:
        scale = np.sqrt(P_max / current_power)
        W_flat *= scale
    return W_flat

# ==========================================
# 4. THUẬT TOÁN PSO (CORE)
# ==========================================
def run_pso(variant='basic'):
    dim = 2 * M * (K + 1) # Kích thước bài toán (Thực + Ảo)
    
    # Khởi tạo
    X = np.random.uniform(-1, 1, (n_particles, dim))
    V = np.random.uniform(-0.1, 0.1, (n_particles, dim))
    
    # Chuẩn hóa công suất ban đầu
    for i in range(n_particles):
        X[i] = check_power_constraint(X[i])
        
    P_best = X.copy()
    P_best_scores = np.array([calculate_fitness(x) for x in X])
    
    G_best = P_best[np.argmax(P_best_scores)].copy()
    G_best_score = np.max(P_best_scores)
    
    history = []
    
    # Vòng lặp tối ưu
    for t in range(n_iterations):
        # Cập nhật tham số theo biến thể
        if variant == 'ipso':
            w = 0.9 - (0.5 * t / n_iterations) # Giảm tuyến tính từ 0.9 -> 0.4
            c1, c2 = 2.0, 2.0
            chi = 1.0
        elif variant == 'cpso':
            w = 1.0 # Trong CPSO w thường để 1 hoặc ẩn đi
            c1, c2 = 2.05, 2.05
            phi = c1 + c2
            chi = 2 / np.abs(2 - phi - np.sqrt(phi**2 - 4*phi)) # ~0.729
        else: # Basic PSO
            w = 0.7 # Hằng số
            c1, c2 = 2.0, 2.0
            chi = 1.0
            
        # Cập nhật Vận tốc và Vị trí
        r1 = np.random.rand(n_particles, dim)
        r2 = np.random.rand(n_particles, dim)
        
        if variant == 'cpso':
            V = chi * (V + c1*r1*(P_best - X) + c2*r2*(G_best - X))
        else:
            V = w*V + c1*r1*(P_best - X) + c2*r2*(G_best - X)
            
        X = X + V
        
        # Xử lý ràng buộc và Đánh giá lại
        for i in range(n_particles):
            X[i] = check_power_constraint(X[i])
            score = calculate_fitness(X[i])
            
            if score > P_best_scores[i]:
                P_best[i] = X[i]
                P_best_scores[i] = score
                
            if score > G_best_score:
                G_best = X[i]
                G_best_score = score
        
        history.append(G_best_score)
        
    return history, G_best
# ==========================================
# 5. CHẠY MÔ PHỎNG VÀ VẼ ĐỒ THỊ
# ==========================================
print("Đang chạy PSO Basic...")
hist_basic, best_basic = run_pso('basic')

print("Đang chạy IPSO (Inertia Weight)...")
hist_ipso, best_ipso = run_pso('ipso')

print("Đang chạy CPSO (Constriction Factor)...")
hist_cpso, best_cpso = run_pso('cpso')

# --- ĐỒ THỊ 1: HỘI TỤ ---
plt.figure(figsize=(10, 6))
plt.plot(hist_basic, label='Basic PSO', linestyle='--')
plt.plot(hist_ipso, label='IPSO (Inertia Weight)', linewidth=2)
plt.plot(hist_cpso, label='CPSO (Constriction Factor)', linewidth=2)
plt.xlabel('Số vòng lặp (Iterations)')
plt.ylabel('Giá trị hàm thích nghi (Fitness Value)')
plt.title('So sánh tốc độ hội tụ giữa các biến thể PSO')
plt.legend()
plt.grid(True)
plt.savefig('convergence_comparison.png') # Lưu ảnh
plt.show()

# --- ĐỒ THỊ 2: BEAMPATTERN (BÚP SÓNG) ---
def plot_beampattern(W_flat, label_str):
    W_complex = W_flat[:M*(K+1)] + 1j * W_flat[M*(K+1):]
    W = W_complex.reshape(M, K+1)
    
    angles = np.linspace(-90, 90, 360) * np.pi / 180
    gains = []
    
    for theta in angles:
        a = steering_vector(theta, M)
        # Tổng hợp tín hiệu tại hướng theta
        gain = np.abs(np.vdot(a, np.sum(W, axis=1)))**2
        gains.append(gain)
    
    gains_db = 10 * np.log10(np.array(gains) + 1e-9)
    plt.plot(np.degrees(angles), gains_db, label=label_str)

plt.figure(figsize=(10, 6))
plot_beampattern(best_basic, 'Basic PSO')
plot_beampattern(best_cpso, 'CPSO (Best Variant)')

# Vẽ các đường thẳng đứng chỉ vị trí User và Target
plt.axvline(x=-30, color='r', linestyle=':', label='User 1 (-30°)')
plt.axvline(x=10, color='r', linestyle=':', label='User 2 (10°)')
plt.axvline(x=40, color='g', linestyle='--', label='Target (40°)')

plt.xlabel('Góc (Độ)')
plt.ylabel('Biên độ búp sóng (dB)')
plt.title('Đồ thị bức xạ búp sóng (Beampattern)')
plt.legend()
plt.grid(True)
plt.ylim(-40, 20)
plt.savefig('beampattern_result.png') # Lưu ảnh
plt.show()
