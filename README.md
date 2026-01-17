# Tối ưu hóa JCAS Multibeam sử dụng PSO

Đồ án Bài tập lớn môn **Kỹ thuật Truyền thông** - Đại học Bách Khoa Hà Nội (HUST).

## 👥 Thành viên Nhóm 33
| Họ và tên | MSSV |
|-----------|------|
| Nguyễn Khánh Toàn | 20235847 |
| Trần Việt Gia Khánh | 20235756 |

## 📌 Giới thiệu
Dự án này nghiên cứu áp dụng thuật toán **Particle Swarm Optimization (PSO)** và các biến thể của nó để giải quyết bài toán tối ưu hóa đa chùm tia (Multibeam) trong hệ thống Tích hợp Truyền thông và Cảm biến (JCAS).

Chúng tôi so sánh hiệu năng của 3 thuật toán:
1. **Basic PSO**: Thuật toán PSO cơ bản.
2. **IPSO (Inertia Weight)**: Biến thể trọng số quán tính.
3. **CPSO (Constriction Factor)**: Biến thể hệ số co.

**Mục tiêu tối ưu hóa:**
* Tối đa hóa tổng tốc độ dữ liệu (Sum Rate).
* Tối đa hóa độ lợi cảm biến (Sensing Gain) tại hướng Radar.
* Giảm thiểu nhiễu búp sóng phụ (Sidelobe).

## 📂 Cấu trúc thư mục
```text
JCAS-Multibeam-PSO/
├── src/
│   ├── python/        # Code mô phỏng chính (JCAS System)
│   └── cpp_tests/     # Code kiểm thử thuật toán PSO (C++)
├── results/           # Chứa ảnh kết quả (Output)
├── requirements.txt   # Danh sách thư viện cần thiết
└── README.md          # Tài liệu hướng dẫn
```

## 🚀 Hướng dẫn chạy Code Python (Mô phỏng JCAS)

### Bước 1: Cài đặt thư viện
```bash
pip install -r requirements.txt
```
### Chạy mô phỏng
```bash
cd src/python
python jcas_pso.py
```
### Kết quả: Chương trình sẽ xuất ra 2 file ảnh trong thư mục results (hoặc thư mục gốc):

convergence_comparison.png: So sánh tốc độ hội tụ.

beampattern_result.png: Đồ thị búp sóng sau khi tối ưu.

## 🛠️ Hướng dẫn chạy Code C++ (Test Thuật toán)
Các file này dùng để kiểm chứng thuật toán PSO trên các hàm test đơn giản (Benchmark functions).

```bash
cd src/cpp_tests
g++ PSOtest.cpp -o pso_test
./pso_test
```

