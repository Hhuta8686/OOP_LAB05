/******************************
Mã sinh viên: 202418997
Họ tên: Ngọc Hương Trà
******************************/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <ctime>

using namespace std;

enum DeviceStatus {
    Active,
    UnderMaintenance,
    Retired
};

string statusToString(DeviceStatus status) {
    switch (status) {
        case Active: return "Active";
        case UnderMaintenance: return "UnderMaintenance";
        case Retired: return "Retired";
        default: return "Unknown";
    }
}

class INetworkable {
public:
    virtual ~INetworkable() {}
    virtual string getIpAddress() const = 0;
    virtual void connect(const string& ipAddress) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
};

class Device {
protected:
    string deviceId;
    string deviceName;
    int yearInUse;
    double purchasePrice;
    DeviceStatus status;

public:
    Device(string id, string name, int year, double price, DeviceStatus st) {
        if (id.empty()) {
            throw invalid_argument("Mã thiết bị không được rỗng.");
        }
        if (price <= 0) {
            throw invalid_argument("Giá mua phải lớn hơn 0.");
        }
        
        time_t t = time(0);
        tm* now = localtime(&t);
        int currentYear = now->tm_year + 1900;
        if (year > currentYear) {
            throw invalid_argument("Năm đưa vào sử dụng không được lớn hơn năm hiện tại.");
        }

        deviceId = id;
        deviceName = name.empty() ? "Unknown" : name;
        yearInUse = year;
        purchasePrice = price;
        status = st;
    }

    virtual ~Device() {}

    string getDeviceId() const { return deviceId; }
    string getDeviceName() const { return deviceName; }
    void setDeviceName(const string& name) { deviceName = name.empty() ? "Unknown" : name; }
    
    int getYearInUse() const { return yearInUse; }
    void setYearInUse(int year) {
        time_t t = time(0);
        tm* now = localtime(&t);
        int currentYear = now->tm_year + 1900;
        if (year > currentYear) throw invalid_argument("Năm không hợp lệ.");
        yearInUse = year;
    }

    double getPurchasePrice() const { return purchasePrice; }
    void setPurchasePrice(double price) {
        if (price <= 0) throw invalid_argument("Giá phải lớn hơn 0.");
        purchasePrice = price;
    }

    DeviceStatus getStatus() const { return status; }
    void setStatus(DeviceStatus st) { status = st; }

    virtual double calculateAnnualMaintenanceCost() = 0;

    virtual void displayInfo() const {
        cout << "[ID: " << deviceId << "] Tên: " << deviceName 
             << ", Năm sử dụng: " << yearInUse 
             << ", Giá: " << (long long)purchasePrice << " VNĐ"
             << ", Trạng thái: " << statusToString(status);
    }
};

class Computer : public Device, public INetworkable {
private:
    int ramCapacity;
    string processorType;
    bool hasDedicatedGpu;
    string ipAddress;

public:
    Computer(string id, string name, int year, double price, DeviceStatus st,
             int ram, string cpu, bool gpu)
        : Device(id, name, year, price, st), ramCapacity(ram), processorType(cpu), hasDedicatedGpu(gpu), ipAddress("") {}

    string getIpAddress() const override { return ipAddress; }
    bool isConnected() const override { return !ipAddress.empty(); }

    void connect(const string& ip) override {
        if (ip.empty()) throw invalid_argument("Địa chỉ IP không được rỗng.");
        if (isConnected()) throw runtime_error("Không cho phép kết nối lại khi thiết bị đang kết nối.");
        ipAddress = ip;
    }

    void disconnect() override {
        ipAddress = "";
    }

    double calculateAnnualMaintenanceCost() override {
        double cost = purchasePrice * 0.05;
        if (hasDedicatedGpu) {
            cost += purchasePrice * 0.02;
        }
        time_t t = time(0);
        tm* now = localtime(&t);
        int age = (now->tm_year + 1900) - yearInUse;
        if (age > 5) {
            cost += purchasePrice * 0.01;
        }
        return cost;
    }

    void displayInfo() const override {
        Device::displayInfo();
        cout << ", RAM: " << ramCapacity << "GB, CPU: " << processorType 
             << ", GPU rời: " << (hasDedicatedGpu ? "Có" : "Không")
             << ", IP: " << (isConnected() ? ipAddress : "Chưa kết nối");
    }
};

class Printer : public Device {
protected:
    string printerType;
    int printedPagesCount;
    bool isColor;

public:
    Printer(string id, string name, int year, double price, DeviceStatus st,
            string type, int pages, bool color)
        : Device(id, name, year, price, st), printerType(type), printedPagesCount(pages), isColor(color) {}

    double calculateAnnualMaintenanceCost() override {
        double cost = purchasePrice * 0.04;
        if (printedPagesCount > 100000) {
            cost += 500000.0;
        }
        if (isColor) {
            cost += 300000.0;
        }
        return cost;
    }

    void displayInfo() const override {
        Device::displayInfo();
        cout << ", Loại: " << printerType << ", Số trang: " << printedPagesCount 
             << ", Màu: " << (isColor ? "Có" : "Không");
    }
};

class NetworkPrinter : public Printer, public INetworkable {
private:
    string ipAddress;

public:
    NetworkPrinter(string id, string name, int year, double price, DeviceStatus st,
                   string type, int pages, bool color)
        : Printer(id, name, year, price, st, type, pages, color), ipAddress("") {}

    string getIpAddress() const override { return ipAddress; }
    bool isConnected() const override { return !ipAddress.empty(); }

    void connect(const string& ip) override {
        if (ip.empty()) throw invalid_argument("Địa chỉ IP không được rỗng.");
        if (isConnected()) throw runtime_error("Không cho phép kết nối lại khi thiết bị đang kết nối.");
        ipAddress = ip;
    }

    void disconnect() override {
        ipAddress = "";
    }

    void displayInfo() const override {
        Printer::displayInfo();
        cout << ", IP: " << (isConnected() ? ipAddress : "Chưa kết nối");
    }
};

class Projector : public Device {
private:
    int lumen;
    int lampUsedHours;

public:
    Projector(string id, string name, int year, double price, DeviceStatus st,
              int lum, int hours)
        : Device(id, name, year, price, st), lumen(lum), lampUsedHours(hours) {}

    double calculateAnnualMaintenanceCost() override {
        double cost = purchasePrice * 0.03;
        if (lampUsedHours > 3000) {
            cost += 1500000.0;
        }
        return cost;
    }

    void displayInfo() const override {
        Device::displayInfo();
        cout << ", Độ sáng: " << lumen << " lumen, Giờ bóng: " << lampUsedHours << "h";
    }
};


class LabRoom {
private:
    string roomId;
    string roomName;
    int capacity;
    vector<Device*> devices;

public:
    LabRoom(string id, string name, int cap) : roomId(id), roomName(name), capacity(cap) {}

    string getRoomId() const { return roomId; }
    string getRoomName() const { return roomName; }

    void addDevice(Device* device) {
        if (device == nullptr) {
            throw invalid_argument("Không chấp nhận đối tượng thiết bị null.");
        }
        for (const auto& d : devices) {
            if (d->getDeviceId() == device->getDeviceId()) {
                throw runtime_error("Thiết bị có mã này đã tồn tại trong phòng.");
            }
        }
        if (devices.size() >= (size_t)capacity) {
            throw runtime_error("Phòng thực hành đã đạt sức chứa tối đa.");
        }
        devices.push_back(device);
    }

    bool removeDevice(string deviceId) {
        auto it = remove_if(devices.begin(), devices.end(), [&](Device* d) {
            return d->getDeviceId() == deviceId;
        });
        if (it != devices.end()) {
            devices.erase(it, devices.end());
            return true;
        }
        return false;
    }

    Device* findDevice(string deviceId) {
        for (auto& d : devices) {
            if (d->getDeviceId() == deviceId) return d;
        }
        return nullptr;
    }

    double calculateAnnualMaintenanceCost() const {
        double total = 0;
        for (const auto& d : devices) {
            total += const_cast<Device*>(d)->calculateAnnualMaintenanceCost();
        }
        return total;
    }

    vector<Device*> getDevicesRequiringMaintenance() const {
        time_t t = time(0);
        tm* now = localtime(&t);
        int currentYear = now->tm_year + 1900;
        vector<Device*> result;
        for (const auto& d : devices) {
            if (d->getStatus() == UnderMaintenance || (currentYear - d->getYearInUse()) > 5) {
                result.push_back(const_cast<Device*>(d));
            }
        }
        return result;
    }

    const vector<Device*>& getDevices() const { return devices; }
};


int main() {
    system("chcp 65001 > nul");

    try {
        Computer* pc1 = new Computer("PC01", "Dell Optiplex", 2022, 15000000, Active, 16, "Intel Core i5", false);
        Computer* pc2 = new Computer("PC02", "MSI Gaming PC", 2018, 25000000, UnderMaintenance, 32, "Intel Core i7", true);

        Printer* printer1 = new Printer("PR01", "HP Laser Printer", 2023, 4000000, Active, "Laser", 120000, false);
        NetworkPrinter* netPrinter = new NetworkPrinter("PR02", "Canon Network Printer", 2024, 6000000, Active, "Inkjet", 50000, true);

        Projector* projector1 = new Projector("PJ01", "Sony Projector", 2020, 12000000, Active, 3500, 3500);

        LabRoom room1("LAB1", "Phòng Lập trình", 5);
        LabRoom room2("LAB2", "Phòng Mạng máy tính", 5);

        room1.addDevice(pc1);
        room1.addDevice(pc2);
        room1.addDevice(printer1);

        room2.addDevice(netPrinter);
        room2.addDevice(projector1);

        cout << "-> Thêm thiết bị vào phòng thành công!\n" << endl;

        try {
            cout << "[Thử nghiệm] Thêm thiết bị trùng mã 'PC01' vào phòng LAB1:" << endl;
            Computer* duplicatePC = new Computer("PC01", "Duplicate PC", 2025, 10000000, Active, 8, "Core i3", false);
            room1.addDevice(duplicatePC);
        } catch (const exception& ex) {
            cout << "Bắt lỗi thành công: " << ex.what() << endl;
        }

        cout << "\n--- DANH SÁCH THIẾT BỊ TRONG " << room1.getRoomName() << " ---" << endl;
        for (const auto& d : room1.getDevices()) {
            cout << " - ";
            d->displayInfo();
            cout << endl;
        }

        cout << "\n--- DANH SÁCH THIẾT BỊ TRONG " << room2.getRoomName() << " ---" << endl;
        for (const auto& d : room2.getDevices()) {
            cout << " - ";
            d->displayInfo();
            cout << endl;
        }

        cout << "\n--- TỔNG CHI PHÍ BẢO TRÌ DỰ KIẾN ---" << endl;
        cout << "Phòng " << room1.getRoomId() << ": " << (long long)room1.calculateAnnualMaintenanceCost() << " VNĐ" << endl;
        cout << "Phòng " << room2.getRoomId() << ": " << (long long)room2.calculateAnnualMaintenanceCost() << " VNĐ" << endl;

        cout << "\n--- DANH SÁCH THIẾT BỊ CẦN BẢO TRÌ Ở PHÒNG " << room1.getRoomId() << " ---" << endl;
        for (const auto& d : room1.getDevicesRequiringMaintenance()) {
            cout << " - [" << d->getDeviceId() << "] " << d->getDeviceName() 
                 << " (Trạng thái: " << statusToString(d->getStatus()) << ", Năm sử dụng: " << d->getYearInUse() << ")" << endl;
        }

        cout << "\n--- KẾT NỐI MẠNG CHO CÁC THIẾT BỊ INETWORKABLE ---" << endl;
        vector<INetworkable*> networkDevices = { pc1, pc2, netPrinter };

        for (size_t i = 0; i < networkDevices.size(); ++i) {
            try {
                string testIp = (i == 0) ? "192.168.1.10" : ((i == 1) ? "192.168.1.11" : "192.168.1.20");
                networkDevices[i]->connect(testIp);
                cout << "Đã kết nối thành công tới IP: " << networkDevices[i]->getIpAddress() << endl;
            } catch (const exception& ex) {
                cout << "Lỗi kết nối: " << ex.what() << endl;
            }
        }

        cout << "\n--- DUYỆT CÁC THIẾT BỊ MẠNG THÔNG QUA INETWORKABLE ---" << endl;
        for (auto* netDev : networkDevices) {
            Device* devObj = dynamic_cast<Device*>(netDev);
            if (devObj != nullptr) {
                cout << "- Thiết bị [" << devObj->getDeviceId() << "] " << devObj->getDeviceName() 
                     << " đang dùng IP: " << netDev->getIpAddress() << endl;
            }
        }

        delete pc1; delete pc2; delete printer1; delete netPrinter; delete projector1;

    } catch (const exception& ex) {
        cout << "Lỗi chương trình: " << ex.what() << endl;
    }

    cout << "\nNhấn Enter để thoát...";
    cin.get();
    return 0;
}