# Báo cáo schematic - Smart Switch v1.1

## 1. Mục tiêu và phạm vi

Tài liệu này mô tả schematic Smart Switch v1.1, các khối phần cứng chính, đường cấp nguồn và ước lượng dòng tiêu thụ phục vụ lựa chọn nguồn. Sơ đồ được chia thành các sheet: `Power_Supply`, `STM32`, `Input` và `Output`.

Các tải điện lưới được đóng cắt qua tiếp điểm relay. Phần điện tử điều khiển nhận nguồn DC 12 V. Schematic không cho biết đầy đủ mã module TFT, dòng cuộn relay theo BOM, hoặc dòng của tải ngoài, do đó các con số tiêu thụ trong báo cáo là ước lượng thiết kế cần được xác nhận bằng datasheet linh kiện/module thực tế và phép đo trên mạch hoàn chỉnh.

## 2. Kiến trúc phần cứng tổng thể

```text
Adapter DC 12 V
   ├── TPS54231 buck ── +5 V ── AMS1117-3.3 ── +3V3
   ├── Cuộn relay 12 V (4 kênh)
   └── Các nhánh tải phụ thuộc thiết kế

STM32F103C8T6
   ├── TFT SPI (ILI9341 theo phân công firmware)
   ├── Touch SPI (XPT2046 theo phân công firmware)
   ├── DHT22 / AM2302, PIR, đầu nối LDR
   ├── 4 nút nhấn
   └── Mạch driver optocoupler + transistor + relay
```

Schematic là nguồn tham chiếu cho kết nối điện. Tên và đặc tính ILI9341/XPT2046 trong tài liệu phân công firmware cần được đối chiếu với module TFT/touch thực tế trước khi lắp ráp; sheet TFT chỉ thể hiện đầu nối tín hiệu, không ghi mã module hay mức tiêu thụ.

## 3. Các khối và linh kiện chính

| Khối            | Linh kiện thể hiện trên schematic                            | Chức năng                                                                                                              |
| --------------- | ------------------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------- |
| Đầu vào DC      | J1 jack DC, F1 fuse 16 V/2.5 A, D3 SS34, C9 10 µF/25 V       | Nhận 12 V; F1 bảo vệ quá dòng; D3 mắc bảo vệ đảo cực theo cấu hình trên sơ đồ; C9 lọc đầu vào.                         |
| Buck 5 V        | U1 TPS54231DR, L1 10 µH/5.6 A, D1 SS34, C2-C8, mạng hồi tiếp | Hạ +12 V xuống +5 V. L1 có dòng định mức ghi 5.6 A; đây là thông số cuộn cảm, không phải dòng tải đầu ra được cam kết. |
| LDO 3.3 V       | U2 AMS1117-3.3, C10-C13                                      | Hạ +5 V thành +3V3 cho MCU và các mạch logic.                                                                          |
| MCU             | STM32F103C8T6, thạch anh Y1 8 MHz, C14-C15 22 pF             | Xử lý điều khiển; có SWD, mạch BOOT0 kéo xuống 10 kΩ và RESET kéo lên 10 kΩ, nút reset và tụ 100 nF.                   |
| Lọc nguồn logic | C17-C24: 100 nF và 1 µF theo từng nhánh                      | Tụ bypass/decoupling giữa +3V3 và GND gần các nhóm mạch.                                                               |
| TFT và touch    | J9 đầu nối 8 chân; J10 đầu nối 5 chân                        | J9 mang SPI và các chân điều khiển TFT; J10 mang SPI và IRQ touch.                                                     |
| Cảm biến        | U5 AM2302/DHT22, J3 PIR, J4 LDR/LM393                        | Đọc nhiệt độ/độ ẩm, chuyển động và tín hiệu ánh sáng analog `LDR_A0`. R21 4.7 kΩ kéo lên đường dữ liệu DHT22.          |
| Nút nhấn        | SW2-SW5, R19/R20/R22/R23 10 kΩ, C27-C30 100 nF               | Bốn nút được kéo lên +3V3, nhấn nối tín hiệu xuống GND; tụ tạo lọc nhiễu/dội phím phần cứng.                           |
| Ngõ ra          | U6-U9 PC817, Q1-Q4 MMBTA42, K1-K4 HF46 12-HS1, D7-D10 SS210  | Cách ly điều khiển qua optocoupler và đóng cắt cuộn relay 12 V bằng transistor; diode dập xung ngược cuộn relay.       |
| LED chỉ thị     | D2/D4/D5 báo các rail; D11-D14 báo channel; D6 LED_STATUS    | Hiển thị trạng thái nguồn và các kênh. Điện trở hạn dòng được thể hiện trên sơ đồ.                                     |
| Đầu ra tải      | J5-J8 terminal 2 chân; J11 terminal AC; RV1 470 V            | Tiếp điểm relay đưa ra các đầu tải. RV1 mắc giữa AC1/AC2 để hạn chế xung quá áp trên đường tải theo ý đồ schematic.    |

**Lưu ý về DHT22:** AM2302/DHT22 dùng giao thức một dây riêng (single-wire), không phải bus I²C dù chân dữ liệu trên symbol có thể mang tên `SDA`. Firmware cần cấu hình GPIO và timing theo DHT22.

**Lưu ý về ngõ ra AC:** J5-J8 được vẽ là tiếp điểm relay AC1/AC2; J11 là đầu vào 220 VAC dùng cho khối chống xung RV1. Thực hiện khoảng cách cách điện, khe hở creepage/clearance, cầu chì phía tải và vỏ bảo vệ theo yêu cầu an toàn điện của PCB và sản phẩm cuối. Không coi 12 V đầu vào là nguồn cấp cho tải AC.

## 4. Kết nối MCU và giao tiếp

| Tín hiệu                                     | Chân MCU theo sheet STM32 | Vai trò dự kiến                                             |
| -------------------------------------------- | ------------------------- | ----------------------------------------------------------- |
| `LDR_A0`                                     | PA0                       | ADC đọc mức ánh sáng                                        |
| `DHT22`                                      | PA1                       | Dữ liệu DHT22                                               |
| `PIR`                                        | PA2                       | Ngõ vào phát hiện chuyển động                               |
| `Button_1`                                   | PA3                       | Nút MODE theo phân công                                     |
| `TFT_CS`                                     | PA4                       | Chip select TFT                                             |
| `TFT_SCK`                                    | PA5                       | SPI clock TFT                                               |
| `TFT_MOSI`                                   | PA7                       | SPI MOSI TFT                                                |
| `TFT_LED`                                    | PA8                       | Điều khiển đèn nền, cần xác nhận khả năng drive theo module |
| `LED_CH1`, `LED_CH2`                         | PA11, PA12                | LED trạng thái kênh                                         |
| `TFT_DC`, `TFT_RST`                          | PB0, PB1                  | Data/command và reset TFT                                   |
| `Button_2`, `Button_3`, `Button_4`           | PB3, PB4, PB5             | Ba nút vật lý còn lại                                       |
| `FAN`, `LIGHT_1`, `DEHUMIDIFIER`             | PB6, PB7, PB8             | Tín hiệu điều khiển relay được gắn nhãn trên sheet MCU      |
| `LED_CH3`, `LED_CH4`                         | PB9, PB10                 | LED trạng thái kênh                                         |
| `T_IRQ`, `T_CS`, `T_SCK`, `T_MISO`, `T_MOSI` | PB11-PB15                 | Giao tiếp touch SPI và tín hiệu IRQ                         |
| `LED_STATUS`                                 | PC13                      | LED trạng thái hệ thống                                     |
| `SWDIO`, `SWCLK`                             | PA13, PA14                | Nạp/chạy debug qua SWD; đưa ra J2 cùng +3V3/GND             |

SPI TFT sử dụng các tín hiệu SCK/MOSI và điều khiển riêng; sheet không thể hiện MISO ở đầu nối TFT. Touch có MOSI/MISO riêng theo J10. Khi cấu hình firmware phải kiểm tra mapping SPI1/SPI2 và alternate-function thực tế trên STM32F103C8T6, đồng thời xác minh mức logic của module.


## 5. Phân tích và tính toán nguồn

### 5.1. Đường nguồn

1. Adapter cấp **+12 V DC** vào J1. F1 được ghi **16 V / 2.5 A**; cần chọn adapter có điện áp 12 V ổn định và dòng đáp ứng tổng tải, còn F1 là giới hạn bảo vệ chứ không đồng nghĩa mạch luôn tiêu thụ 2.5 A.
2. U1 TPS54231 tạo đường **+5 V** từ +12 V. Mạng hồi tiếp thể hiện R2 = 2.49 kΩ nối tiếp R5 = 49.9 kΩ ở nhánh trên và R6 = 10 kΩ ở nhánh dưới. Lấy điện áp tham chiếu danh định `Vref ≈ 0.8 V`, ta có `Vout ≈ 0.8 × (1 + (2.49 + 49.9)/10) = 4.99 V`, phù hợp mục tiêu +5 V. Điện áp thực tế còn phụ thuộc sai số điện trở, IC và layout; cần đo rail khi vận hành.
3. U2 AMS1117-3.3 tạo **+3V3** từ +5 V. Các tụ đầu vào/đầu ra giúp ổn định và giảm nhiễu; đặt tụ bypass gần chân nguồn tải.
4. Cuộn K1-K4 dùng đường **+12 V**, vì vậy dòng cuộn relay không đi qua LDO 3.3 V. Tiếp điểm relay cách ly tải với logic; khi relay đóng, dòng tải do nguồn ngoài/tải AC cung cấp.

> Điện áp +5 V ở trên là giá trị tính theo mạng hồi tiếp và điện áp tham chiếu danh định; nên đo +5 V khi chạy không tải và khi tải lớn để xác nhận mạch đã lắp.

### 5.2. Ước lượng tải đường 3.3 V

Theo yêu cầu phân tích, lấy dòng STM32 khoảng **60 mA** và TFT khoảng **100 mA** làm giả thiết. Các dòng của touch, sensor, LED và tổn hao ngoại vi dưới đây chỉ là dự phòng sơ bộ, không thay thế datasheet.

| Tải trên +3V3                                |                             Dòng dự kiến |
| -------------------------------------------- | ---------------------------------------: |
| STM32F103C8T6 và mạch dao động/GPIO          |                        60 mA (giả thiết) |
| TFT và đèn nền                               | 100 mA (giả thiết; phải xác nhận module) |
| XPT2046 / mạch touch                         |                         10 mA (dự phòng) |
| DHT22, PIR và phần mạch cảm biến lấy từ +3V3 |                         15 mA (dự phòng) |
| LED chỉ thị và GPIO khác                     |                         10 mA (dự phòng) |
| **Tổng tải tính sơ bộ**                      |                               **195 mA** |

Cộng 25% dự phòng cho sai số, thay đổi tải và dòng khởi động:

```text
I_3V3,design = 195 mA × 1.25 ≈ 244 mA
P_3V3       = 3.3 V × 0.244 A ≈ 0.81 W
```

Ở AMS1117, dòng vào xấp xỉ dòng tải nếu bỏ qua dòng tĩnh nhỏ. Công suất nhiệt của LDO:

```text
P_loss ≈ (Vin - Vout) × Iout
       ≈ (5.0 V - 3.3 V) × 0.244 A
       ≈ 0.415 W
```

Mức tiêu tán khoảng 0.42 W cần được kiểm tra theo package, diện tích đồng tản nhiệt, nhiệt độ môi trường và giới hạn nhiệt junction của linh kiện. Nếu dòng TFT/backlight thực cao hơn 100 mA hoặc PCB tản nhiệt kém, cân nhắc dùng buck 3.3 V riêng hoặc module TFT có nguồn/driver backlight riêng thay vì tăng tải AMS1117.

### 5.3. Ước lượng tải đường 5 V và đầu vào 12 V

Nếu phần tải +3V3 được cấp qua AMS1117, dòng lấy từ 5 V gần **244 mA** theo dự phòng trên. Công suất đầu ra buck cho nhánh này khoảng:

```text
P_5V ≈ 5 V × 0.244 A = 1.22 W
I_12V,buck ≈ P_5V / (12 V × η)
```

Với hiệu suất giả định η = 85%, dòng 12 V tương ứng khoảng **120 mA** (chưa tính các tải 5 V khác nếu có). Bốn relay nằm trên 12 V. Do chưa có thông số cuộn relay xác nhận, dùng ví dụ minh họa **30 mA/cuộn**: bốn cuộn đồng thời cần khoảng **120 mA**, công suất 12 V là **1.44 W**. Nếu thiết kế cho phép bật đồng thời mọi relay, tổng dòng đầu vào minh họa là khoảng `120 mA + 120 mA = 240 mA`, chưa kể LED, mạch điều khiển và tổn hao. Cần thay 30 mA bằng giá trị datasheet của HF46-12-HS1 đúng phiên bản.

Chọn adapter không chỉ dựa trên dòng MCU/TFT: hãy cộng dòng buck quy đổi, dòng tối đa cuộn relay khi các kênh cùng bật, LED và mọi tải DC khác, sau đó dùng hệ số dự phòng khoảng 1.3-1.5. Với phép tính sơ bộ hiện có, adapter **12 V / 1 A** có vẻ dư cho phần mạch logic và bốn cuộn relay, nhưng chỉ chốt sau khi xác nhận dòng cuộn, tải phụ và giới hạn của buck/đầu nối. Fuse F1 2.5 A không phải khuyến nghị adapter 2.5 A.

### 5.4. Kiểm tra thực nghiệm đề xuất

- Đo +12 V, +5 V và +3V3 ở trạng thái idle, TFT sáng tối đa, và khi bốn relay cùng đóng.
- Đo dòng từ adapter bằng ampe kế nối tiếp hoặc nguồn bench có giới hạn dòng; ghi lại cả dòng khởi động nếu nguồn sụt.
- Kiểm tra nhiệt độ AMS1117 sau thời gian hoạt động ổn định trong điều kiện TFT/backlight và MCU tải cao.
- Xác nhận điện áp logic, cực tính relay, trạng thái reset/khởi động và khả năng điều khiển chân `TFT_LED`.
- Đối chiếu dòng cuộn relay, dòng backlight TFT và giới hạn dòng adapter với datasheet đúng mã linh kiện/module trước khi phát hành BOM.

## 6. Mạch nút và cảm biến

Bốn nút có điện trở kéo lên 10 kΩ tới +3V3 và tụ 100 nF xuống GND. Khi nhấn, GPIO bị kéo xuống mức thấp; firmware nên cấu hình active-low và có debounce bằng timer/phần mềm. Hằng số RC danh định là `τ = R × C = 10 kΩ × 100 nF = 1 ms`; đây là lọc nhiễu ngắn, không thay thế debounce phần mềm cho thời gian dội phím cơ.

DHT22 có điện trở kéo lên 4.7 kΩ trên đường DATA. PIR dùng đầu nối 3 chân (3V3, tín hiệu, GND). J4 LDR/LM393 đưa ra `LDR_A0`; cần xác định trên module thực tế chân này là analog output và điện áp cực đại không vượt quá VDDA/ADC của STM32.
