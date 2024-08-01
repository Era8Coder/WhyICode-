#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <set>
#include <tuple>

using namespace std;

class Bank {
public:
    string name;
    int netAmount;
    multiset<string> paymentTypes;
};

void addTransaction(unordered_map<string, unordered_map<string, int>>& graph, const string& u, const string& v, int amount) {
    graph[u][v] += amount;
}

unordered_map<string, int> calculateNetAmounts(const unordered_map<string, unordered_map<string, int>>& graph) {
    unordered_map<string, int> netAmounts;
    for (const auto& u : graph) {
        for (const auto& v : u.second) {
            netAmounts[u.first] -= v.second;
            netAmounts[v.first] += v.second;
        }
    }
    return netAmounts;
}

vector<tuple<string, string, int, string>> minimizeTransactions(const unordered_map<string, int>& netAmounts, const vector<Bank>& banks) {
    priority_queue<pair<int, string>> maxHeap;              // It is used to keep bank with positive Net Amounts    => Positive for Creditors
    priority_queue<pair<int, string>> minHeap;              // It is used to keep bank with negative Net Amounts    => Negativ for Debitors

    for (const auto& person : netAmounts) {             // Itersting over Net Amounts Map 
        if (person.second > 0) {                // If positive keep it in MaxHeap
            maxHeap.push(make_pair(person.second, person.first));
        } else if (person.second < 0) {         // iF negative keep it in MinHeap
            minHeap.push(make_pair(-person.second, person.first));      
        }
    }

    vector<tuple<string, string, int, string>> transactions;        // transactions is a tuple to store the result, where each transaction is represented as a tuple (debtor, creditor, settledAmount, paymentMethod)

    while (!maxHeap.empty() && !minHeap.empty()) {
        auto maxCredit = maxHeap.top(); maxHeap.pop();          // Pop the top element from maxHeap (largest creditor) into maxCredit
        auto maxDebit = minHeap.top(); minHeap.pop();           // Pop the top element from minHeap (largest debtor) into maxDebit

        int settledAmount = min(maxCredit.first, maxDebit.first);   // amount to settle is the minimum of the largest credit and the largest debit

        string paymentMethod;
        for (const auto& bank : banks) {            // Iterating through the banks vector to find the bank that matches the creditor 
            if (bank.name == maxCredit.second) {
                for (const auto& type : bank.paymentTypes) {    // Checking for a common payment type between the creditor and the debtor 
                    if (banks[netAmounts.at(maxDebit.second)].paymentTypes.count(type)) {
                        paymentMethod = type;
                        break;
                    }
                }
                break;
            }
        }

        transactions.push_back(make_tuple(maxDebit.second, maxCredit.second, settledAmount, paymentMethod));    // Add the transaction to the transactions vector

        if (maxCredit.first > settledAmount) {          // If the creditor's amount is not fully settled, push the remaining amount back into maxHeap.
            maxHeap.push(make_pair(maxCredit.first - settledAmount, maxCredit.second));
        }
        if (maxDebit.first > settledAmount) {           // If the debtor's amount is not fully settled, push the remaining amount back into minHeap.
            minHeap.push(make_pair(maxDebit.first - settledAmount, maxDebit.second));
        }
    }

    return transactions;
}

void printTransactions(const vector<tuple<string, string, int, string>>& transactions) {
    for (const auto& t : transactions) {
        cout << get<0>(t) << " pays " << get<1>(t) << " amount " << get<2>(t) << " via " << get<3>(t) << endl;
    }
}

int main() {
    unordered_map<string, unordered_map<string, int>> graph;
    
    vector<Bank> banks = {
        {"A", 0, {"Type1", "Type2"}},
        {"B", 0, {"Type1"}},
        {"C", 0, {"Type2"}},
        {"D", 0, {"Type1"}},
        {"E", 0, {"Type2"}}
    };
    
    addTransaction(graph, "A", "B", 50);
    addTransaction(graph, "A", "C", 30);
    addTransaction(graph, "B", "C", 20);
    addTransaction(graph, "D", "B", 40);
    addTransaction(graph, "E", "B", 70);

    unordered_map<string, int> netAmounts = calculateNetAmounts(graph);
    vector<tuple<string, string, int, string>> transactions = minimizeTransactions(netAmounts, banks);
    
    printTransactions(transactions);

    return 0;
}
