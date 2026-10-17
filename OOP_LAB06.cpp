/*********************************
Mã sinh viên: 202418997
Họ tên: Ngọc Hương Trà
*********************************/

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

enum class DeviceStatus {
    Available,         
    Borrowed,           
    UnderMaintenance,  
    Retired            
};

string statusToString(DeviceStatus status) {
    switch (status) {
        case DeviceStatus::Available: return "Available";
        case DeviceStatus::Borrowed: return "Borrowed";
        case DeviceStatus::UnderMaintenance: return "UnderMaintenance";
        case DeviceStatus::Retired: return "Retired";
    }
    return "Unknown";
}

class BorrowingException : public runtime_error {
public:
    explicit BorrowingException(const string& msg) : runtime_error(msg) {}
};

class DeviceUnavailableException : public BorrowingException {
public:
    DeviceUnavailableException(const string& deviceId, DeviceStatus status)
        : BorrowingException("Thiet bi '" + deviceId + "' hien khong san sang (Trang thai: " + statusToString(status) + ").") {}
};

class BorrowLimitExceededException : public BorrowingException {
public:
    BorrowLimitExceededException(const string& borrowerId, int maxDevices)
        : BorrowingException("Nguoi muon '" + borrowerId + "' da dat so luong thiet bi toi da (" + to_string(maxDevices) + ").") {}
};

class InvalidBorrowPeriodException : public BorrowingException {
public:
    InvalidBorrowPeriodException(const string& deviceId, int requested, int maxAllowed)
        : BorrowingException("Thoi han muon (" + to_string(requested) + " tiet) vuot qua gioi han (" + to_string(maxAllowed) + " tiet) cua thiet bi '" + deviceId + "'.") {}
};

class DuplicateDeviceException : public BorrowingException {
public:
    explicit DuplicateDeviceException(const string& deviceId)
        : BorrowingException("Thiet bi '" + deviceId + "' da ton tai trong phieu muon.") {}
};

class DeviceNotInSlipException : public BorrowingException {
public:
    explicit DeviceNotInSlipException(const string& deviceId)
        : BorrowingException("Thiet bi '" + deviceId + "' khong co trong phieu muon nay.") {}
};

class DeviceAlreadyReturnedException : public BorrowingException {
public:
    explicit DeviceAlreadyReturnedException(const string& deviceId)
        : BorrowingException("Thiet bi '" + deviceId + "' da duoc tra truo c do.") {}
};

class InvalidReturnDateException : public BorrowingException {
public:
    explicit InvalidReturnDateException(const string& msg) : BorrowingException(msg) {}
};

class Device {
protected:
    string deviceId;
    string name;
    int yearOfManufacture;
    double purchasePrice;
    DeviceStatus status;
    int maxAllowedBorrowPeriods;

public:
    Device(string id, string name, int year, double price, int maxPeriods)
        : deviceId(id), name(name), yearOfManufacture(year), purchasePrice(price), maxAllowedBorrowPeriods(maxPeriods) {
        if (id.empty()) throw invalid_argument("Ma thiet bi khong duoc de rong.");
        if (price <= 0) throw invalid_argument("Gia mua phai lon hon 0.");
        if (year > 2026) throw invalid_argument("Nam dua vao su dung khong hop le.");
        status = DeviceStatus::Available;
    }

    virtual ~Device() = default;

    string getDeviceId() const { return deviceId; }
    string getName() const { return name; }
    int getYearOfManufacture() const { return yearOfManufacture; }
    double getPurchasePrice() const { return purchasePrice; }
    DeviceStatus getStatus() const { return status; }
    int getMaxAllowedBorrowPeriods() const { return maxAllowedBorrowPeriods; }

    void setStatus(DeviceStatus s) { status = s; }

    virtual double calculateAnnualMaintenanceCost() const = 0;

    virtual void display() const {
        cout << "[" << deviceId << "] " << name << " - Nam SX: " << yearOfManufacture 
             << " - Gia: " << fixed << setprecision(0) << purchasePrice 
             << " - Trang thai: " << statusToString(status) << endl;
    }
};

class AirConditionerRemote : public Device {
public:
    AirConditionerRemote(string id, string name, int year, double price)
        : Device(id, name, year, price, 6) {}

    double calculateAnnualMaintenanceCost() const override {
        return purchasePrice * 0.05;
    }
};

class Microphone : public Device {
public:
    Microphone(string id, string name, int year, double price)
        : Device(id, name, year, price, 3) {}

    double calculateAnnualMaintenanceCost() const override {
        return purchasePrice * 0.10;
    }
};

class ProjectorRemote : public Device {
public:
    ProjectorRemote(string id, string name, int year, double price)
        : Device(id, name, year, price, 6) {}

    double calculateAnnualMaintenanceCost() const override {
        return purchasePrice * 0.08;
    }
};

class ClassRoom {
private:
    string roomId;
    string roomName;
    int capacity;
    vector<shared_ptr<Device>> devices;

public:
    ClassRoom(string id, string name, int cap) : roomId(id), roomName(name), capacity(cap) {}

    void addDevice(shared_ptr<Device> device) {
        if (!device) throw invalid_argument("Thiet bi khong duoc null.");
        for (const auto& d : devices) {
            if (d->getDeviceId() == device->getDeviceId())
                throw invalid_argument("Thiet bi da ton tai trong phong.");
        }
        devices.push_back(device);
    }

    double calculateAnnualMaintenanceCost() const {
        double total = 0;
        for (const auto& d : devices) {
            total += d->calculateAnnualMaintenanceCost(); 
        }
        return total;
    }
};

class IBorrower {
public:
    virtual ~IBorrower() = default;
    virtual string getBorrowerId() const = 0;
    virtual string getFullName() const = 0;
    virtual int getMaximumDevices() const = 0;
};

class Lecturer : public IBorrower {
private:
    string borrowerId;
    string fullName;
public:
    Lecturer(string id, string name) : borrowerId(id), fullName(name) {}
    string getBorrowerId() const override { return borrowerId; }
    string getFullName() const override { return fullName; }
    int getMaximumDevices() const override { return 3; }
};

class TeachingAssistant : public IBorrower {
private:
    string borrowerId;
    string fullName;
public:
    TeachingAssistant(string id, string name) : borrowerId(id), fullName(name) {}
    string getBorrowerId() const override { return borrowerId; }
    string getFullName() const override { return fullName; }
    int getMaximumDevices() const override { return 2; }
};

class BorrowItem {
private:
    shared_ptr<Device> device;
    int borrowedDate; 
    int fromPeriod;
    int toPeriod;
    int returnedDate; 
    bool isReturned;

public:
    BorrowItem(shared_ptr<Device> dev, int date, int fromP, int toP)
        : device(dev), borrowedDate(date), fromPeriod(fromP), toPeriod(toP), returnedDate(0), isReturned(false) {
        if (!dev) throw invalid_argument("Thiet bi null.");
        if (toP < fromP) throw invalid_argument("Tiet ket thuc phai >= tiet bat dau.");
    }

    shared_ptr<Device> getDevice() const { return device; }
    bool getIsReturned() const { return isReturned; }
    int getBorrowedDate() const { return borrowedDate; }
    int getReturnedDate() const { return returnedDate; }

    void returnDevice(int retDate) {
        if (isReturned) throw DeviceAlreadyReturnedException(device->getDeviceId());
        if (retDate < borrowedDate) throw InvalidReturnDateException("Ngay tra khong duoc truoc ngay muon.");
        
        returnedDate = retDate;
        isReturned = true;
        device->setStatus(DeviceStatus::Available);
    }
};

class BorrowingSlip {
private:
    string slipId;
    shared_ptr<IBorrower> borrower;
    string teachingRoom;
    int borrowedDate;
    vector<BorrowItem> items;

public:
    BorrowingSlip(string id, shared_ptr<IBorrower> b, string room, int date)
        : slipId(id), borrower(b), teachingRoom(room), borrowedDate(date) {
        if (id.empty() || !b) throw invalid_argument("Thong tin phieu muon khong hop le.");
    }

    void addDevice(shared_ptr<Device> device, int fromPeriod, int toPeriod) {
        if (!device) throw invalid_argument("Thiet bi null.");

        int requestedPeriods = toPeriod - fromPeriod + 1;
        if (requestedPeriods <= 0 || requestedPeriods > device->getMaxAllowedBorrowPeriods()) {
            throw InvalidBorrowPeriodException(device->getDeviceId(), requestedPeriods, device->getMaxAllowedBorrowPeriods());
        }

        for (const auto& item : items) {
            if (item.getDevice()->getDeviceId() == device->getDeviceId())
                throw DuplicateDeviceException(device->getDeviceId());
        }

        if ((int)items.size() >= borrower->getMaximumDevices()) {
            throw BorrowLimitExceededException(borrower->getBorrowerId(), borrower->getMaximumDevices());
        }

        if (device->getStatus() != DeviceStatus::Available) {
            throw DeviceUnavailableException(device->getDeviceId(), device->getStatus());
        }

  
        BorrowItem item(device, borrowedDate, fromPeriod, toPeriod);
        device->setStatus(DeviceStatus::Borrowed);
        items.push_back(item);
    }

    double returnDevice(const string& deviceId, int returnedDate) {
        for (auto& item : items) {
            if (item.getDevice()->getDeviceId() == deviceId) {
                item.returnDevice(returnedDate);
                int delayDays = returnedDate - borrowedDate;
                return (delayDays > 0) ? delayDays * 20000.0 : 0.0;
            }
        }
        throw DeviceNotInSlipException(deviceId);
    }
};


void runTestCase(const string& testName, void (*func)()) {
    cout << "\n[Kiem thu] " << testName << endl;
    try {
        func();
        cout << "  -> [THAT BAI] Khong phat sinh ngoai le nhu ky vong." << endl;
    }
    catch (const BorrowingException& ex) {
        cout << "  -> [THANH CONG] Da bat ngoai le nghiep vu:" << endl;
        cout << "     " << ex.what() << endl;
    }
    catch (const exception& ex) {
        cout << "  -> [LOI HE THONG]: " << ex.what() << endl;
    }
}

int main() {
    cout << "=================================================" << endl;
    cout << "   HE THONG QUAN LY THIET BI VA PHIEU MUON" << endl;
    cout << "=================================================\n" << endl;


    cout << "--- 1. LUONG CHAY THANH CONG ---" << endl;
    try {
        auto lecturer = make_shared<Lecturer>("GV001", "Nguyen Van A");
        auto pjRemote = make_shared<ProjectorRemote>("PJ01", "Dieu khien may chieu", 2022, 500000);
        auto mic = make_shared<Microphone>("MC01", "Micro khong day", 2023, 1500000);

        BorrowingSlip slip("SLIP001", lecturer, "Phong 302-A2", 20261017);

        slip.addDevice(pjRemote, 1, 3);
        slip.addDevice(mic, 1, 3);
        cout << "-> Them PJ01 va MC01 vao phieu thanh cong." << endl;

        slip.returnDevice("PJ01", 20261017);
        cout << "-> Tra thiet bi PJ01 dung han thanh cong." << endl;

        double fee = slip.returnDevice("MC01", 20261019); // Tra tre 2 ngay
        cout << "-> Tra MC01 tre 2 ngay. Phi phat tre: " << fixed << setprecision(0) << fee << " VND" << endl;
        cout << "\n[Thanh cong] Toan bo thao tac luong chuan hoan tat!\n" << endl;
    }
    catch (const exception& ex) {
        cout << "Loi: " << ex.what() << endl;
    }

    cout << "--- 2. KIEM THU CAC TINH HUONG PHAT SINH NGOAI LE ---" << endl;

    runTestCase("1. Muon thiet bi dang Bao tri (UnderMaintenance)", []() {
        auto lecturer = make_shared<Lecturer>("GV002", "Tran Van B");
        auto acRemote = make_shared<AirConditionerRemote>("AC02", "Dieu khien dieu hoa", 2020, 250000);
        acRemote->setStatus(DeviceStatus::UnderMaintenance);

        BorrowingSlip slip("SLIP002", lecturer, "Phong 101", 20261017);
        slip.addDevice(acRemote, 1, 2);
    });

    runTestCase("2. Tro giang muon thiet bi thu 3 (Gioi han toi da la 2)", []() {
        auto ta = make_shared<TeachingAssistant>("TA001", "Le Van C");
        auto d1 = make_shared<ProjectorRemote>("PJ02", "May chieu 2", 2022, 400000);
        auto d2 = make_shared<AirConditionerRemote>("AC03", "Dieu hoa 3", 2022, 300000);
        auto d3 = make_shared<Microphone>("MC02", "Micro 2", 2022, 800000);

        BorrowingSlip slip("SLIP003", ta, "Phong 201", 20261017);
        slip.addDevice(d1, 1, 2);
        slip.addDevice(d2, 1, 2);
        slip.addDevice(d3, 1, 2);
    });

    runTestCase("3. Them cung 1 Micro hai lan vao mot phieu muon", []() {
        auto lecturer = make_shared<Lecturer>("GV003", "Pham Van D");
        auto mic = make_shared<Microphone>("MC03", "Micro 3", 2023, 1200000);

        BorrowingSlip slip("SLIP004", lecturer, "Phong 405", 20261017);
        slip.addDevice(mic, 1, 2);
        slip.addDevice(mic, 1, 2);
    });

    runTestCase("4. Muon Micro 5 tiet (Gioi han toi da la 3 tiet)", []() {
        auto lecturer = make_shared<Lecturer>("GV004", "Hoang Van E");
        auto mic = make_shared<Microphone>("MC04", "Micro 4", 2023, 1200000);

        BorrowingSlip slip("SLIP005", lecturer, "Phong 102", 20261017);
        slip.addDevice(mic, 1, 5);
    });

    runTestCase("5. Tra thiet bi khong nam trong phieu muon", []() {
        auto lecturer = make_shared<Lecturer>("GV005", "Do Van F");
        BorrowingSlip slip("SLIP006", lecturer, "Phong 202", 20261017);

        slip.returnDevice("PJ999", 20261017);
    });

    runTestCase("6. Tra cung mot thiet bi 2 lan", []() {
        auto lecturer = make_shared<Lecturer>("GV006", "Vu Van G");
        auto mic = make_shared<Microphone>("MC05", "Micro 5", 2022, 1000000);

        BorrowingSlip slip("SLIP007", lecturer, "Phong 303", 20261017);
        slip.addDevice(mic, 1, 2);
        slip.returnDevice("MC05", 20261017);
        slip.returnDevice("MC05", 20261017);
    });

    runTestCase("7. Ngay tra truo c ngay muon", []() {
        auto lecturer = make_shared<Lecturer>("GV007", "Bui Van H");
        auto mic = make_shared<Microphone>("MC06", "Micro 6", 2022, 1000000);

        BorrowingSlip slip("SLIP008", lecturer, "Phong 101", 20261017);
        slip.addDevice(mic, 1, 2);
        slip.returnDevice("MC06", 20261016);
    });

    cout << " HOAN THANH KIEM THU!" << endl;

    return 0;
}