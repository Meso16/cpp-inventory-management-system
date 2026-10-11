#include <bits/stdc++.h>
#include <iomanip>
using namespace std;

struct Item {
    string id;
    string name;
    int quantity;
    double price;
};

const string DATA_FILE = "inventory.tsv";

static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

vector<Item> load_data() {
    vector<Item> items;
    ifstream f(DATA_FILE);
    if (!f.is_open()) return items;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;

        vector<string> parts;
        string cur;
        for (char c : line) {
            if (c == '\t') {
                parts.push_back(cur);
                cur.clear();
            } else cur.push_back(c);
        }
        parts.push_back(cur);

        if (parts.size() < 4) continue;

        Item it;
        it.id = parts[0];
        it.name = parts[1];

        try { it.quantity = stoi(parts[2]); }
        catch (...) { it.quantity = 0; }

        try { it.price = stod(parts[3]); }
        catch (...) { it.price = 0.0; }

        items.push_back(it);
    }
    return items;
}

void save_data(const vector<Item>& items) {
    ofstream f(DATA_FILE, ios::trunc);
    if (!f.is_open()) {
        cerr << "Failed to open " << DATA_FILE << " for writing.\n";
        return;
    }
    for (const auto &it : items) {
        f << it.id << '\t' << it.name << '\t'
          << it.quantity << '\t'
          << fixed << setprecision(2) << it.price << '\n';
    }
}

Item* find_item_by_id(vector<Item>& items, const string& id) {
    for (auto &it : items)
        if (it.id == id) return &it;
    return nullptr;
}

void add_item(vector<Item>& items) {
    cout << "\nAdd new item\n";
    cout << "Enter item ID: ";
    string id; getline(cin, id); id = trim(id);

    if (id.empty()) { cout << "ID cannot be empty.\n"; return; }
    if (find_item_by_id(items, id)) {
        cout << "Item with ID already exists.\n";
        return;
    }

    cout << "Enter item name: ";
    string name; getline(cin, name); name = trim(name);

    if (name.empty()) { cout << "Name cannot be empty.\n"; return; }

    cout << "Enter quantity: ";
    string qstr; getline(cin, qstr);

    int quantity = 0;
    try {
        quantity = stoi(qstr);
        if (quantity < 0) throw runtime_error("neg");
    } catch (...) {
        cout << "Invalid quantity.\n"; return;
    }

    cout << "Enter price: ";
    string pstr; getline(cin, pstr);

    double price = 0.0;
    try {
        price = stod(pstr);
        if (price < 0) throw runtime_error("neg");
    } catch (...) {
        cout << "Invalid price.\n"; return;
    }

    items.push_back({id, name, quantity, price});
    save_data(items);
    cout << "Item added successfully.\n";
}

void view_items(const vector<Item>& items) {
    cout << "\nInventory items:\n";

    if (items.empty()) {
        cout << "No items in inventory.\n";
        return;
    }

    cout << left << setw(12) << "ID" << " "
         << left << setw(30) << "Name" << " "
         << right << setw(6) << "Qty" << " "
         << right << setw(10) << "Price" << "\n";

    cout << string(60, '-') << "\n";

    for (const auto &it : items) {
        cout << left << setw(12) << it.id << " "
             << left << setw(30) << it.name << " "
             << right << setw(6) << it.quantity << " "
             << right << setw(10) << fixed << setprecision(2) << it.price << "\n";
    }
}

void update_item(vector<Item>& items) {
    cout << "\nEnter item ID to update: ";
    string id; getline(cin, id); id = trim(id);

    Item* it = find_item_by_id(items, id);
    if (!it) {
        cout << "Item not found.\n"; return;
    }

    cout << "Current data: Qty=" << it->quantity
         << ", Price=" << it->price << "\n";

    cout << "New quantity (blank = keep): ";
    string q; getline(cin, q);

    if (!q.empty()) {
        try {
            int newQ = stoi(q);
            if (newQ < 0) throw runtime_error("neg");
            it->quantity = newQ;
        } catch (...) { cout << "Invalid quantity.\n"; return; }
    }

    cout << "New price (blank = keep): ";
    string p; getline(cin, p);

    if (!p.empty()) {
        try {
            double newP = stod(p);
            if (newP < 0) throw runtime_error("neg");
            it->price = newP;
        } catch (...) { cout << "Invalid price.\n"; return; }
    }

    save_data(items);
    cout << "Item updated.\n";
}

void remove_item(vector<Item>& items) {
    cout << "\nEnter item ID to remove: ";
    string id; getline(cin, id);

    auto it = find_if(items.begin(), items.end(), [&](auto &x){ return x.id == id; });

    if (it == items.end()) {
        cout << "Item not found.\n"; return;
    }

    cout << "Are you sure you want to delete '" << it->name << "'? (y/N): ";
    string c; getline(cin, c);

    if (c == "y" || c == "Y") {
        items.erase(it);
        save_data(items);
        cout << "Item deleted.\n";
    } else {
        cout << "Cancelled.\n";
    }
}

string to_lower_copy(const string& s) {
    string out = s;
    transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

void search_items(const vector<Item>& items) {
    cout << "\nEnter text to search: ";
    string q; getline(cin, q);

    q = to_lower_copy(q);

    vector<Item> results;
    for (auto &it : items) {
        if (to_lower_copy(it.id) == q ||
            to_lower_copy(it.name).find(q) != string::npos)
            results.push_back(it);
    }

    if (results.empty()) {
        cout << "No items found.\n"; return;
    }

    cout << "Found " << results.size() << " result(s):\n";
    view_items(results);
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    vector<Item> items = load_data();

    while (true) {
        cout << "\n========== Inventory Menu ==========\n";
        cout << "1. Add new item\n";
        cout << "2. View all items\n";
        cout << "3. Update item\n";
        cout << "4. Remove item\n";
        cout << "5. Search items\n";
        cout << "6. Clear screen\n";
        cout << "0. Exit\n";
        cout << "Select option: ";

        string ch;
        getline(cin, ch);

        if (ch == "1") add_item(items);
        else if (ch == "2") view_items(items);
        else if (ch == "3") update_item(items);
        else if (ch == "4") remove_item(items);
        else if (ch == "5") search_items(items);
        else if (ch == "6") clear_screen();
        else if (ch == "0") break;
        else cout << "Invalid choice.\n";
    }

    return 0;
}
