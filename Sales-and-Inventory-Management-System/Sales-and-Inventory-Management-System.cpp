#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>

using namespace std;

const string DATA_FILE = "retail_products_cpp.txt";

struct Product {
    string name;
    double price;
    int stock;
};

struct SaleRequest {
    string productId;
    int quantity; // positive = sale, negative = return
};

class RetailSystem {
private:
    unordered_map<string, Product> products;
    queue<SaleRequest> salesQueue;
    stack<SaleRequest> transactionStack;

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    bool isValidPositiveInt(int value) {
        return value > 0;
    }

    bool isValidPositiveDouble(double value) {
        return value > 0;
    }

    void processQueuedSales() {
        if (salesQueue.empty()) {
            return;
        }

        SaleRequest req = salesQueue.front();

        if (products.find(req.productId) != products.end() &&
            products[req.productId].stock >= req.quantity) {
            products[req.productId].stock -= req.quantity;
            transactionStack.push(req);
            salesQueue.pop();

            cout << "Queued sale processed successfully: "
                << req.quantity << " unit(s) of '"
                << products[req.productId].name << "'.\n";
        }
    }

public:
    void addProduct() {
        string id, name;
        double price;
        int stock;

        cout << "\nEnter Product ID: ";
        cin >> id;
        clearInput();

        if (products.find(id) != products.end()) {
            cout << "Product ID already exists.\n";
            return;
        }

        cout << "Enter Product Name: ";
        getline(cin, name);

        cout << "Enter Product Price: ";
        if (!(cin >> price) || !isValidPositiveDouble(price)) {
            cout << "Invalid price.\n";
            clearInput();
            return;
        }

        cout << "Enter Stock Quantity: ";
        if (!(cin >> stock) || !isValidPositiveInt(stock)) {
            cout << "Invalid stock quantity.\n";
            clearInput();
            return;
        }

        products[id] = { name, price, stock };
        cout << "Product added successfully.\n";
    }

    void processSale() {
        string id;
        int qty;

        cout << "\nEnter Product ID to sell: ";
        cin >> id;

        if (products.find(id) == products.end()) {
            cout << "Product not found.\n";
            return;
        }

        cout << "Enter Quantity: ";
        if (!(cin >> qty) || !isValidPositiveInt(qty)) {
            cout << "Invalid quantity.\n";
            clearInput();
            return;
        }

        if (products[id].stock < qty) {
            cout << "Not enough stock. Sale request added to queue.\n";
            salesQueue.push({ id, qty });
            return;
        }

        products[id].stock -= qty;
        transactionStack.push({ id, qty });

        cout << "Sold " << qty << " unit(s) of '"
            << products[id].name << "'.\n";
    }

    void processReturn() {
        string id;
        int qty;

        cout << "\nEnter Product ID to return: ";
        cin >> id;

        if (products.find(id) == products.end()) {
            cout << "Product not found.\n";
            return;
        }

        cout << "Enter Quantity: ";
        if (!(cin >> qty) || !isValidPositiveInt(qty)) {
            cout << "Invalid quantity.\n";
            clearInput();
            return;
        }

        products[id].stock += qty;
        transactionStack.push({ id, -qty });

        cout << "Returned " << qty << " unit(s) of '"
            << products[id].name << "'.\n";

        processQueuedSales();
    }

    void generateReport() {
        if (products.empty()) {
            cout << "\nNo products available.\n";
            return;
        }

        cout << "\n========== Product Inventory Report ==========\n";
        cout << left << setw(12) << "ID"
            << setw(25) << "Name"
            << setw(12) << "Stock"
            << setw(12) << "Price" << "\n";
        cout << string(60, '-') << "\n";

        for (const auto& p : products) {
            cout << left << setw(12) << p.first
                << setw(25) << p.second.name
                << setw(12) << p.second.stock
                << fixed << setprecision(2) << setw(12) << p.second.price
                << "\n";
        }
    }

    void viewLastTransaction() {
        if (transactionStack.empty()) {
            cout << "No transactions available.\n";
            return;
        }

        SaleRequest t = transactionStack.top();
        string action = (t.quantity > 0) ? "Sold" : "Returned";
        int qty = abs(t.quantity);

        if (products.find(t.productId) != products.end()) {
            cout << action << " " << qty << " unit(s) of '"
                << products[t.productId].name << "'.\n";
        }
        else {
            cout << action << " " << qty << " unit(s) of deleted product ID: "
                << t.productId << ".\n";
        }
    }

    void searchProduct() {
        string id;
        cout << "\nEnter Product ID to search: ";
        cin >> id;

        if (products.find(id) == products.end()) {
            cout << "Product not found.\n";
            return;
        }

        Product p = products[id];
        cout << "\nProduct Found:\n";
        cout << "ID: " << id << "\n";
        cout << "Name: " << p.name << "\n";
        cout << "Price: " << fixed << setprecision(2) << p.price << "\n";
        cout << "Stock: " << p.stock << "\n";
    }

    void updateProduct() {
        string id;
        cout << "\nEnter Product ID to update: ";
        cin >> id;
        clearInput();

        if (products.find(id) == products.end()) {
            cout << "Product not found.\n";
            return;
        }

        string newName;
        double newPrice;
        int newStock;

        cout << "Enter New Product Name: ";
        getline(cin, newName);

        cout << "Enter New Price: ";
        if (!(cin >> newPrice) || !isValidPositiveDouble(newPrice)) {
            cout << "Invalid price.\n";
            clearInput();
            return;
        }

        cout << "Enter New Stock Quantity: ";
        if (!(cin >> newStock) || newStock < 0) {
            cout << "Invalid stock quantity.\n";
            clearInput();
            return;
        }

        products[id] = { newName, newPrice, newStock };
        cout << "Product updated successfully.\n";
    }

    void deleteProduct() {
        string id;
        cout << "\nEnter Product ID to delete: ";
        cin >> id;

        if (products.find(id) == products.end()) {
            cout << "Product not found.\n";
            return;
        }

        products.erase(id);
        cout << "Product deleted successfully.\n";
    }

    void undoLastTransaction() {
        if (transactionStack.empty()) {
            cout << "No transaction to undo.\n";
            return;
        }

        SaleRequest last = transactionStack.top();
        transactionStack.pop();

        if (products.find(last.productId) == products.end()) {
            cout << "Cannot undo. Product no longer exists.\n";
            return;
        }

        if (last.quantity > 0) {
            products[last.productId].stock += last.quantity;
            cout << "Undo successful: Restored " << last.quantity
                << " unit(s) to stock.\n";
        }
        else {
            int returnedQty = abs(last.quantity);

            if (products[last.productId].stock < returnedQty) {
                cout << "Undo failed: insufficient stock to reverse return.\n";
                transactionStack.push(last);
                return;
            }

            products[last.productId].stock -= returnedQty;
            cout << "Undo successful: Reversed return of "
                << returnedQty << " unit(s).\n";
        }
    }

    void save() {
        ofstream file(DATA_FILE);
        if (!file.is_open()) {
            cout << "Error saving file.\n";
            return;
        }

        for (const auto& p : products) {
            file << p.first << "|"
                << p.second.name << "|"
                << p.second.price << "|"
                << p.second.stock << "\n";
        }

        file.close();
    }

    void load() {
        ifstream file(DATA_FILE);
        if (!file.is_open()) {
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string id, name, priceStr, stockStr;

            getline(ss, id, '|');
            getline(ss, name, '|');
            getline(ss, priceStr, '|');
            getline(ss, stockStr, '|');

            if (!id.empty() && !name.empty() &&
                !priceStr.empty() && !stockStr.empty()) {
                Product p;
                p.name = name;
                p.price = stod(priceStr);
                p.stock = stoi(stockStr);
                products[id] = p;
            }
        }

        file.close();
    }

    void showMenu() {
        cout << "\n========== Retail Management System ==========\n";
        cout << "1. Add Product\n";
        cout << "2. Sell Product\n";
        cout << "3. Return Product\n";
        cout << "4. Inventory Report\n";
        cout << "5. Last Transaction\n";
        cout << "6. Search Product\n";
        cout << "7. Update Product\n";
        cout << "8. Delete Product\n";
        cout << "9. Undo Last Transaction\n";
        cout << "10. Exit\n";
        cout << "Choose an option: ";
    }

    void run() {
        load();

        int choice;
        while (true) {
            showMenu();

            if (!(cin >> choice)) {
                cout << "Invalid input. Please enter a number.\n";
                clearInput();
                continue;
            }

            switch (choice) {
            case 1:
                addProduct();
                break;
            case 2:
                processSale();
                break;
            case 3:
                processReturn();
                break;
            case 4:
                generateReport();
                break;
            case 5:
                viewLastTransaction();
                break;
            case 6:
                searchProduct();
                break;
            case 7:
                updateProduct();
                break;
            case 8:
                deleteProduct();
                break;
            case 9:
                undoLastTransaction();
                break;
            case 10:
                save();
                cout << "Data saved. Exiting program.\n";
                return;
            default:
                cout << "Invalid option. Please try again.\n";
            }
        }
    }
};

int main() {
    RetailSystem system;
    system.run();
    return 0;
}