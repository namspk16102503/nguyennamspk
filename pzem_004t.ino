#include <WiFi.h>
#include <WebServer.h>
#include <PZEM004Tv30.h>

// Thông tin WiFi
const char* ssid = "S23"; // Tên mạng WiFi
const char* password = "88888888"; // Mật khẩu WiFi
// khai bao bien

// Đối tượng PZEM. Sử dụng để đo điện áp và dòng điện
HardwareSerial pzemSerial(2); // Sử dụng UART2 để giao tiếp với PZEM
PZEM004Tv30 pzem(&pzemSerial, 16, 17); // TX -> GPIO16, RX -> GPIO17

// Tạo đối tượng WebServer, chạy trên cổng 80
WebServer server(80);

// Trạng thái của 10 hộ dân
String customerStatus[10] = {"Đã đóng", "Đã đóng", "Chưa đóng", "Chưa đóng", "Đã đóng",
                             "Đã đóng", "Chưa đóng", "Đã đóng", "Chưa đóng", "Đã đóng"}; // Tình trạng đóng tiền
String cbStatus[10] = {"On", "On", "Off", "Off", "On", "On", "Off", "On", "Off", "On"}; // Trạng thái CB

// Hàm đọc điện áp từ PZEM
float readVoltage() {
  return pzem.voltage(); // Trả về giá trị điện áp
}

// Hàm đọc dòng điện từ PZEM
float readCurrent() {
  return pzem.current(); // Trả về giá trị dòng điện
}

// Hàm cập nhật trạng thái hộ dân
void updateStatus(int index, String newCustomerStatus, String newCbStatus) {
  if (index >= 0 && index < 10) { // Kiểm tra chỉ số hợp lệ
    customerStatus[index] = newCustomerStatus; // Cập nhật trạng thái thanh toán
    cbStatus[index] = newCbStatus; // Cập nhật trạng thái CB
    Serial.println("Cập nhật thành công!");
  } else {
    Serial.println("Index không hợp lệ!"); // Báo lỗi nếu chỉ số không hợp lệ
  }
}

// Trang chính hiển thị danh sách hộ dân
void handleMainPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>"; // Đặt mã hóa ký tự là UTF-8
  html += "<title>Bảng Theo Dõi Tình Trạng Đóng Tiền Điện</title>";
  html += "<link href='https://fonts.googleapis.com/css2?family=Roboto:wght@400;500;700&display=swap' rel='stylesheet'>";
  html += "<style>";
  
  // Nền trang đồng nhất
  html += "body {";
  html += "  font-family: 'Roboto', sans-serif;";
  html += "  text-align: center;";
  html += "  margin: 0;";
  html += "  padding: 0;";
  html += "  background-color: #f0f8ff;"; // Nền xanh nhạt đồng nhất
  html += "  font-size: 1.4em;"; // Tăng kích thước chữ tổng thể
  html += "}";

  html += "h1 {";
  html += "  font-size: 3.5em;";
  html += "  color: #004d99;";
  html += "  margin-top: 20px;";
  html += "  text-shadow: 2px 2px 4px rgba(0, 0, 0, 0.2);"; // Hiệu ứng đổ bóng nhẹ
  html += "}";

  html += "table {";
  html += "  border-collapse: collapse;";
  html += "  width: 95%;";
  html += "  margin: 20px auto;";
  html += "  background-color: #ffffff;"; // Nền trắng cho bảng
  html += "  box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);"; // Hiệu ứng bóng
  html += "}";

  html += "th, td {";
  html += "  border: 1px solid #ddd;";
  html += "  padding: 20px;";
  html += "  font-size: 1.5em;"; // Tăng kích thước chữ
  html += "}";

  html += "th {";
  html += "  background-color: #004d99;";
  html += "  color: white;";
  html += "  text-transform: uppercase;";
  html += "}";

  html += "tr:hover {";
  html += "  background-color: #f1f1f1;"; // Màu khi rê chuột
  html += "}";

  html += "a {";
  html += "  text-decoration: none;";
  html += "  color: #007bff;"; // Màu liên kết
  html += "  font-weight: bold;";
  html += "  font-size: 1.3em;"; // Tăng kích thước chữ liên kết
  html += "}";

  html += "a:hover {";
  html += "  text-decoration: underline;";
  html += "  color: #0056b3;"; // Màu khi hover
  
  html += "}";

  // Thêm kiểu cho trạng thái CB
  html += ".cb-on {";
  html += "  color: #28a745;"; // Màu xanh lá cây
  html += "  font-weight: bold;";
  html += "  font-size: 2em;"; // Tăng kích thước chữ cho trạng thái CB
  html += "}";

  html += ".cb-off {";
  html += "  color: #dc3545;"; // Màu đỏ
  html += "  font-weight: bold;";
  html += "  font-size: 2em;"; // Tăng kích thước chữ cho trạng thái CB
  html += "}";
  // Thêm kiểu cho trạng thái thanh toán
  html += ".paid {";
  html += "  color: #28a745;"; // Màu xanh lá cây cho "Đã đóng"
  html += "  font-weight: bold;";
  html += "  font-size: 2em;"; // Tăng kích thước chữ cho trạng thái CB
  html += "}";

  html += ".unpaid {";
  html += "  color: #dc3545;"; // Màu đỏ cho "Chưa đóng"
  html += "  font-weight: bold;";
  html += "  font-size: 2em;"; // Tăng kích thước chữ cho trạng thái CB
  html += "}";
  html += "</style>";
  html += "</head><body>";

  html += "<h1>BẢNG THEO DÕI TÌNH TRẠNG ĐÓNG TIỀN ĐIỆN</h1>";
  html += "<table>";
  html += "<tr><th>Hộ dân</th><th>Tình trạng đóng tiền điện</th><th>Trạng thái CB</th></tr>";

  /// Thêm dữ liệu hộ dân
  for (int i = 0; i < 10; i++) {
    html += "<tr>";
    html += "<td><a href='/detail?h=" + String(i) + "'>Hộ " + String(i + 1) + "</a></td>"; // Link đến chi tiết

    // Tình trạng thanh toán
    if (customerStatus[i] == "Đã đóng") {
      html += "<td class='paid'>" + customerStatus[i] + "</td>"; // Hiển thị màu xanh
    } else {
      html += "<td class='unpaid'>" + customerStatus[i] + "</td>"; // Hiển thị màu đỏ
    }

    // Định dạng trạng thái CB dựa trên giá trị
    if (cbStatus[i] == "On") {
      html += "<td class='cb-on'>" + cbStatus[i] + "</td>";
    } else {
      html += "<td class='cb-off'>" + cbStatus[i] + "</td>";
    }

    html += "</tr>";
  }

  html += "</table>";
  html += "</body></html>";

  server.send(200, "text/html; charset=utf-8", html); // Gửi HTML đến trình duyệt
}



// Trang chi tiết hiển thị thông tin cụ thể của hộ dân
void handleDetailPage() {
  if (!server.hasArg("h")) { // Kiểm tra tham số h (số hộ dân) có tồn tại không
    server.send(400, "text/plain; charset=utf-8", "Thiếu tham số hộ dân");
    return;
  }

  int h = server.arg("h").toInt(); // Lấy giá trị của tham số h
  if (h < 0 || h >= 10) { // Kiểm tra tham số hợp lệ
    server.send(400, "text/plain; charset=utf-8", "Tham số hộ dân không hợp lệ");
    return;
  }

  // Đọc dữ liệu từ cảm biến
  float voltage = readVoltage();
  float current = readCurrent();
  
  float power = voltage * current; // Tính công suất
  float energy = 150.0; // Giá trị năng lượng (giả định)
  float totalBill = energy * 2000; // Tính tiền điện (giá giả định 2000 VND/kWh)

  // Tạo trang HTML hiển thị chi tiết
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>"; // Đặt mã hóa UTF-8
  html += "<title>Chi Tiết Hộ Dân</title>";
  html += "<link href='https://fonts.googleapis.com/css2?family=Roboto:wght@400;500;700&display=swap' rel='stylesheet'>"; // Font
  html += "<style>";
  
  // CSS đã chỉnh sửa để tăng kích thước chữ và bảng
  html += "body {";
  html += "  font-family: 'Roboto', sans-serif;";
  html += "  text-align: center;";
  html += "  font-size: 2em;"; // Tăng kích thước chữ cho toàn bộ trang
  html += "}";

  html += "table {";
  html += "  border-collapse: collapse;";
  html += "  width: 80%;"; // Tăng chiều rộng bảng
  html += "  margin: 20px auto;";
  html += "  font-size: 1.8em;"; // Tăng kích thước chữ trong bảng
  html += "}";

  html += "th, td {";
  html += "  border: 1px solid black;";
  html += "  padding: 16px;"; // Tăng padding trong ô
  html += "  text-align: center;";
  html += "}";

  html += "th {";
  html += "  background-color: #f2f2f2;";
  html += "}";

  html += "a {";
  html += "  text-decoration: none;";
  html += "  color: blue;";
  html += "  font-weight: bold;";
  html += "  font-size: 2em;"; // Tăng kích thước chữ cho liên kết
  html += "}";

  html += "a:hover {";
  html += "  text-decoration: underline;";
  html += "}";

  html += "</style>";
  html += "</head><body>";
  html += "<h1>Thông Tin Chi Tiết Hộ " + String(h + 1) + "</h1>";
  html += "<table>";
  html += "<tr><th>Thông số</th><th>Giá trị</th></tr>";
  html += "<tr><td>Điện áp</td><td id='voltage'>" + String(voltage) + " V</td></tr>";
  html += "<tr><td>Dòng điện</td><td id='current'>" + String(current) + " A</td></tr>";
  html += "<tr><td>Công suất</td><td>" + String(power) + " W</td></tr>";
  html += "<tr><td>Tổng năng lượng tiêu thụ</td><td>" + String(energy) + " kWh</td></tr>";
  html += "<tr><td>Tổng tiền điện</td><td>" + String(totalBill) + " VND</td></tr>";
  html += "</table>";
  html += "<br><a href='/'>Quay lại</a>";
  html += "<script>setInterval(() => {fetch('/update').then(res => res.json()).then(data => {document.getElementById('voltage').innerText = data.voltage + ' V';document.getElementById('current').innerText = data.current + ' A';});}, 5000);</script>"; // Cập nhật dữ liệu theo thời gian thực
  html += "</body></html>";

  server.send(200, "text/html; charset=utf-8", html); // Gửi nội dung HTML về client
}


// Endpoint trả dữ liệu JSON để cập nhật
void handleUpdate() {
  float voltage = readVoltage(); // Lấy điện áp từ cảm biến
  float current = readCurrent(); // Lấy dòng điện từ cảm biến
  String json = "{\"voltage\":" + String(voltage) + ",\"current\":" + String(current) + "}"; // Tạo chuỗi JSON
  server.send(200, "application/json; charset=utf-8", json); // Gửi JSON về client
}

// Cấu hình server và các endpoint
void setup() {
  Serial.begin(115200); // Khởi tạo Serial để debug
  pzemSerial.begin(9600, SERIAL_8N1, 16, 17); // Cấu hình giao tiếp với PZEM

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Đang kết nối WiFi...");
  }
  Serial.println("Đã kết nối WiFi");
  Serial.println(WiFi.localIP()); // In địa chỉ IP

  // Đăng ký các endpoint
  server.on("/", handleMainPage); // Endpoint cho trang chính
  server.on("/detail", handleDetailPage); // Endpoint cho trang chi tiết
  server.on("/update", handleUpdate); // Endpoint trả về JSON
  server.begin(); // Bắt đầu server
  Serial.println("Server đã khởi động");
}

// Vòng lặp chính
void loop() {
  server.handleClient(); // Xử lý các yêu cầu từ client
}
