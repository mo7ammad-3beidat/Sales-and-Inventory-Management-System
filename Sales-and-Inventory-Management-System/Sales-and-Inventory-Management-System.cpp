#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <stack>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// File to store product data
const string DATA_FILE = "retail_products_cpp.txt";

// Struct for product summary
struct ProductSummary {
    string name;
    int availableStock;
    double price;
};

// Class for product
class Product {
public:
    string name;
    double price;
    int stock;
};

// Struct for sale/return transaction
struct SaleRequest {
    string productId;
    int quantity; // Positive for sale, negative for return
};

// Data structures
unordered_map<string, Product> products;
queue<SaleRequest> salesQueue; // FIFO for pending sales
stack<SaleRequest> transactionStack; // LIFO for last transactions

// Function to add product
void addProduct() {
    string id, name;
    double price;
    int stock;

    cout << "Product ID: "; cin >> id;
    cin.ignore();
    cout << "Product Name: "; getline(cin, name);
    cout << "Price: "; cin >> price;
    cout << "Stock Quantity: "; cin >> stock;

    products[id] = Product{ name, price, stock };
    cout << "Product added successfully.\n";
}

// Function to process sale
void processSale() {
    string id;
    int qty;
    cout << "Product ID to sell: "; cin >> id;
    cout << "Quantity: "; cin >> qty;

    if (products.find(id) == products.end()) {
        cout << "Product not found.\n";
        return;
    }

    if (products[id].stock < qty) {
        cout << "Not enough stock. Adding request to sales queue.\n";
        salesQueue.push(SaleRequest{ id, qty });
        return;
    }

    products[id].stock -= qty;
    transactionStack.push(SaleRequest{ id, qty });
    cout << "Sold " << qty << " units of '" << products[id].name << "'.\n";
}

// Function to process return
void processReturn() {
    string id;
    int qty;
    cout << "Product ID to return: "; cin >> id;
    cout << "Quantity: "; cin >> qty;

    if (products.find(id) == products.end()) {
        cout << "Product not found.\n";
        return;
    }

    products[id].stock += qty;
    transactionStack.push(SaleRequest{ id, -qty });
    cout << "Returned " << qty << " units of '" << products[id].name << "'.\n";

    // Process queued sales
    if (!salesQueue.empty()) {
        SaleRequest req = salesQueue.front();
        if (products[req.productId].stock >= req.quantity) {
            products[req.productId].stock -= req.quantity;
            salesQueue.pop();
            transactionStack.push(req);
            cout << "Queued sale processed: " << req.quantity << " units of '"
                << products[req.productId].name << "'.\n";
        }
    }
}

// Function to generate report
void generateReport() {
    cout << "\nProduct Inventory Report:\n";
    cout << left << setw(10) << "ID" << setw(20) << "Name"
        << setw(10) << "Stock" << setw(10) << "Price" << "\n";
    for (auto& p : products) {
        cout << setw(10) << p.first
            << setw(20) << p.second.name
            << setw(10) << p.second.stock
            << setw(10) << fixed << setprecision(2) << p.second.price
            << "\n";
    }
}

// Function to view last transaction
void viewLastTransaction() {
    if (transactionStack.empty()) {
        cout << "No transactions yet.\n";
        return;
    }
    SaleRequest t = transactionStack.top();
    string action = t.quantity > 0 ? "Sold" : "Returned";
    int qty = abs(t.quantity);
    cout << action << " " << qty << " units of '" << products[t.productId].name << "'.\n";
}

// Save products to file
void save() {
    ofstream file(DATA_FILE);
    for (auto& p : products) {
        file << p.first << "|" << p.second.name << "|" << p.second.price << "|" << p.second.stock << "\n";
    }
    file.close();
}

// Load products from file
void load() {
    ifstream file(DATA_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string id, name, priceStr, stockStr;
        getline(ss, id, '|');
        getline(ss, name, '|');
        getline(ss, priceStr, '|');
        getline(ss, stockStr, '|');

        Product p;
        p.name = name;
        p.price = stod(priceStr);
        p.stock = stoi(stockStr);
        products[id] = p;
    }
    file.close();
}

// Main menu loop
int main() {
    load();
    while (true) {
        cout << "\n1 Add Product  2 Sell Product  3 Return Product  4 Inventory Report  5 Last Transaction  6 Exit\n";
        int choice; cin >> choice;
        switch (choice) {
        case 1: addProduct(); break;
        case 2: processSale(); break;
        case 3: processReturn(); break;
        case 4: generateReport(); break;
        case 5: viewLastTransaction(); break;
        case 6: save(); return 0;
        default: cout << "Invalid option.\n"; break;
        }
    }
}
