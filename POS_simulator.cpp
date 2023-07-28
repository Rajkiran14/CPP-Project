#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>

using namespace std;
struct Product {
    std::string name;
    double price;
    int stock;
    int sold;
};

class User {
protected:
    std::string username;
    std::string password;
    friend class pos;

public:
    User(const std::string& username, const std::string& password)
        : username(username), password(password) {}

    
};

class Customer : public User {
private:
    std::vector<Product> shoppingCart;

public:
    Customer(const std::string& username, const std::string& password)
        : User(username, password) {}

};

class Seller : public User {
private:
    std::vector<Product> products;

public:
    Seller(const std::string& username, const std::string& password)
        : User(username, password) {}

    
};


class SalesManager {
public:
    SalesManager() {}
    friend class pos;

    
};

class POS {
private:
   
    std::map<std::string, std::string> customers;
    std::map<std::string, std::string> sellers;
    std::vector<Product> products;
    std::vector<Product> shoppingCart;

    std::string currentUserType;
    std::string currentUsername;

    const std::string customerFilePath = "cut.txt";
    void readCustomerDataFromFile() {
    std::ifstream file(customerFilePath);
    if (file.is_open()) {
        std::string username, password;
        while (file >> username >> password) {
            customers[username] = password;
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open the file for reading." << std::endl;
    }
}

void writeCustomerDataToFile() {
    std::ofstream file(customerFilePath);
    if (file.is_open()) {
        for (const auto& entry : customers) {
            file << entry.first << " " << entry.second << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open the file for writing." << std::endl;
    }
}
public:
    POS() {
        
        products.push_back({"mango", 10.0, 100, 0});
        products.push_back({"banana", 15.0, 50, 0});
        products.push_back({"bottle", 20.0, 75, 0});
        products.push_back({"bag", 20.0, 75, 0});
        products.push_back({"book", 20.0, 75, 0});

        readCustomerDataFromFile();        
        sellers["raj"] = "raj";
    }
     

    void registerCustomer(const std::string& username, const std::string& password) {
        customers[username] = password;
        writeCustomerDataToFile();

    }


    bool login(const std::string& username, const std::string& password, const std::string& userType) {
        try {
            if (userType == "customer") {
                auto it = customers.find(username);
                if (it != customers.end() && it->second == password) {
                    currentUserType = "customer";
                    currentUsername = username;
                    return true;
                }
            } else if (userType == "seller") {
                auto it = sellers.find(username);
                if (it != sellers.end() && it->second == password) {
                    currentUserType = "seller";
                    currentUsername = username;
                    return true;
                }
            }
            throw std::runtime_error("Invalid username or password.");
        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
            return false;
        }
    }

    void displayProducts() const {
        std::cout << "Available Products:\n";
        for (const auto& product : products) {
            std::cout << product.name << " - $" << product.price << " - Stock: " << product.stock << "\n";
        }
    }

    void addToCart(int productIndex, int quantity) {
        try {
            if (productIndex >= 0 && productIndex < products.size()) {
                if (products[productIndex].stock >= quantity) {
                    shoppingCart.push_back({products[productIndex].name, products[productIndex].price, quantity, 0});
                    std::cout << "Added " << quantity << " units of " << products[productIndex].name << " to the cart.\n";
                    products[productIndex].stock -= quantity;
                } else {
                    throw std::runtime_error("Insufficient stock for " + products[productIndex].name + ".");
                }
            } else {
                throw std::out_of_range("Invalid product index.");
            }
        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void removeFromCart(int productIndex, int quantity) {
        try {
            if (productIndex >= 0 && productIndex < shoppingCart.size()) {
                if (shoppingCart[productIndex].stock >= quantity) {
                    shoppingCart[productIndex].stock -= quantity;
                    int productStockIndex = findProductIndex(shoppingCart[productIndex].name);
                    products[productStockIndex].stock += quantity;
                    if (shoppingCart[productIndex].stock == 0) {
                        shoppingCart.erase(shoppingCart.begin() + productIndex);
                    }
                    std::cout << "Removed " << quantity << " units of " << shoppingCart[productIndex].name << " from the cart.\n";
                } else {
                    throw std::invalid_argument("Invalid quantity to remove. Quantity in cart: " + std::to_string(shoppingCart[productIndex].stock));
                }
            } else {
                throw std::out_of_range("Invalid product index.");
            }
        } catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
        }
    }

    void displayCart() const {
        double total = 0.0;
        std::cout << "Shopping Cart:\n";
        for (const auto& product : shoppingCart) {
            std::cout << product.name << " - $" << product.price << " - Quantity: " << product.stock << "\n";
            total += product.price * product.stock;
        }
        std::cout << "Total: $" << total << "\n";
    }

    void displayStocksAndSalesPercentage() const {
        int totalStock = 0;
        for (const auto& product : products) {
            totalStock += product.stock;
        }
        std::cout << "Total Stock: " << totalStock << "\n";

        int totalSold = 0;
        for (const auto& product : products) {
            totalSold += product.sold;
        }

        double salesPercentage = (static_cast<double>(totalSold) / (totalStock + totalSold)) * 100;
        std::cout << "Sales Percentage: " << salesPercentage << "%\n";
    }

    void checkout() {
        double total = 0.0;
        std::cout << "Bill:\n";
        for (const auto& product : shoppingCart) {
            int productIndex = findProductIndex(product.name);
            std::cout << product.name << " - $" << product.price << " - Quantity: " << product.stock
                      << " - Subtotal: $" << product.price * product.stock << "\n";
            total += product.price * product.stock;
            products[productIndex].sold += product.stock;
        }
        std::cout << "Total: $" << total << "\n";
        shoppingCart.clear();
           // Ask customer to pay
        double payment;
        std::cout << "Enter the payment amount: $";
        std::cin >> payment;

        // calculate the change
        double change = payment - total;
        if (change >= 0) {
            std::cout << "Change: $" << change << "\n";
            shoppingCart.clear();
        } else {
            std::cout << "Insufficient payment. Transaction cancelled.\n";
    }
        
    }

private:
    int findProductIndex(const std::string& productName) const {
        for (int i = 0; i < products.size(); ++i) {
            if (products[i].name == productName) {
                return i;
            }
        }
        return -1;
    }

    
};

int main() {
    POS pos;

    int choice;
    std::string username, password;
    std::string userType;

    do {
        std::cout << "\nMenu:\n";
        std::cout << "1. Register new customer\n";
        std::cout << "2. Customer Sign In\n";
        std::cout << "3. Seller Sign In\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::cout << "Register Customer\n";
                std::cout << "Enter a username: ";
                std::cin >> username;
                std::cout << "Enter a password: ";
                std::cin >> password;

                pos.registerCustomer(username, password);
                std::cout << "Customer registered successfully!\n";
                break;
            }
             case 2: {
                std::cout << "Customer Sign In\n";
                std::cout << "Enter your username: ";
                std::cin >> username;
                std::cout << "Enter your password: ";
                std::cin >> password;
                userType = "customer";
                if (pos.login(username, password, userType)) {
                    int customerChoice;
                    do {
                        std::cout << "\nCustomer Menu:\n";
                        std::cout << "1. Display Products\n";
                        std::cout << "2. Add to Cart\n";
                        std::cout << "3. Remove from Cart\n";
                        std::cout << "4. Display Cart\n";
                        std::cout << "5. Checkout\n";
                        std::cout << "0. Logout\n";
                        std::cout << "Enter your choice: ";
                        std::cin >> customerChoice;

                        switch (customerChoice) {
                            case 1:
                                pos.displayProducts();
                                break;
                            case 2: {
                                int productIndex, quantity;
                                std::cout << "Enter the product index to add to the cart: ";
                                std::cin >> productIndex;
                                std::cout << "Enter the quantity: ";
                                std::cin >> quantity;
                                pos.addToCart(productIndex, quantity);
                                break;
                            }
                            case 3: {
                                int productIndex, quantity;
                                std::cout << "Enter the product index to remove from the cart: ";
                                std::cin >> productIndex;
                                std::cout << "Enter the quantity: ";
                                std::cin >> quantity;
                                pos.removeFromCart(productIndex, quantity);
                                break;
                            }
                            case 4:
                                pos.displayCart();
                                break;
                            case 5:
                                pos.checkout();
                                break;
                            case 0:
                                std::cout << "Logging out...\n";
                                break;
                            default:
                                std::cout << "Invalid choice. Try again.\n";
                                break;
                        }
                    } while (customerChoice != 0);
                } 
                break;
            }
            case 3: {
                std::cout << "Seller Sign In\n";
                std::cout << "Enter your username: ";
                std::cin >> username;
                std::cout << "Enter your password: ";
                std::cin >> password;
                userType = "seller";
                if (pos.login(username, password, userType)) {
                    int sellerChoice;
                    do {
                        std::cout << "\nSeller Menu:\n";
                        std::cout << "1. Display Products\n";
                        std::cout << "2. Display Stocks and Sales Percentage\n";
                        std::cout << "0. Logout\n";
                        std::cout << "Enter your choice: ";
                        std::cin >> sellerChoice;

                        switch (sellerChoice) {
                            case 1:
                                pos.displayProducts();
                                break;
                            case 2:
                                pos.displayStocksAndSalesPercentage();
                                break;
                            case 0:
                                std::cout << "Logging out...\n";
                                break;
                            default:
                                std::cout << "Invalid choice. Try again.\n";
                                break;
                        }
                    } while (sellerChoice != 0);
                } 
                break;
            }
            case 0:
                std::cout << "Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Try again.\n";
                break;
        }
    } while (choice != 0);

    return 0;
}