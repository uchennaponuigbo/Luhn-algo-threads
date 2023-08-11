#include <iostream>
#include<string>
#include <sstream>
#include<chrono>
#include<thread>
#include <mutex>

using std::string;
using std::cout;
using std::cin;
using std::cout;
using std::endl;
using std::thread;
using std::ref;

void luhnMulti(string);
void luhnSingle(string);

string intToString(int);
void verifyLength(string, bool&);
void getCardName(char, string&);

//thread functions
void doubleSecondDigit(string, int&);
void oddDigitSums(string, int&);

//protects the threads from accessing the neccssary data at the same time and causing corruption
std::mutex mutex;

enum CardType
{
    AmericanExpress = 3,
    Visa = 4,
    Master = 5,
    Discover = 6,
    Unknown = 0
};

int main()
{ 
    cout << "Input:\n1 for Multi Thread\n2 for Single Thread\n3 to exit\n\n";
    int choice;
    cin >> choice;
    while (choice == 1 || choice == 2)
    {
        cout << "Input your credit card\n";
        string card; //input card
        cin >> card;
        if (choice == 1)
        {
            cout << "Multi Thread Execution\n";
            luhnMulti(card);
        }
        else if (choice == 2)
        {
            cout << "Single Thread Execution\n";
            luhnSingle(card);
        }
        cout << "\nInput:\n1 for Multi Thread\n2 for Single Thread\n3 to exit\n\n";
        cin >> choice;
    }
    cout << "\nClosing this Program" << endl;
}

string intToString(int a)
{
    std::ostringstream temp;
    temp << a;
    return temp.str();
}

void verifyLength(string card, bool &value)
{
    value = card.length() >= 13 && card.length() <= 19;
}

void getCardName(char d, string& industryCard)
{
    int num = d - '0';
    if (num == AmericanExpress)
        industryCard = "American Express";
    else if (num == Visa)
        industryCard = "Visa";
    else if (num == Master)
        industryCard = "Master";
    else if (num == Discover)
        industryCard = "Discover";
    else
        industryCard = "Unkown";
}

void luhnMulti(string card)
{
    bool cardLength;
    thread cardCheck(verifyLength, card, ref(cardLength));

    cardCheck.join();
    if (!cardLength)
    {
        cout << "Invalid Length. Size " << card.length() << "\n";
        return;
    }
    auto start = std::chrono::high_resolution_clock::now();
    //Task 2
    string industryCard = "";
    thread whatCard(getCardName, card[0], ref(industryCard));

    //task 3, double every second digit from right to left
    int singleDigitSum = 0;
    thread doubleDigits(doubleSecondDigit, card, ref(singleDigitSum));

    //task 4, add all digits in odd places from right to left
    int oddDigitSum = 0;
    thread oddSums(oddDigitSums, card, ref(oddDigitSum));

    //combine all threads
    whatCard.join();
    doubleDigits.join();
    oddSums.join();

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long milliseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000;
    //cout << singleDigitSum + oddDigitSum << std::endl;
    if ((singleDigitSum + oddDigitSum) % 10 == 0)
        cout << card << " is Valid." << " Card Type: "
        << industryCard << " " << milliseconds << "ms\n";
    else
        cout << card << " is Invalid. " << " Card Type: "
        << industryCard << " " << milliseconds << "ms\n";
}

void doubleSecondDigit(string card, int& sum) //threads
{
    if ((card.length() - 1) % 2 != 0)
        card.pop_back();
    for (int i = card.length() - 1; i >= 0; i -= 2)
    {
        //if (i % 2 == 0)
        //{
            int digit = card[i] - '0'; //convert char to int
            int doubleDigit = digit * 2;
            if (doubleDigit >= 10)
            {
                string splitDigit = intToString(doubleDigit);
                //mutex.lock(); 
                for (size_t j = 0; j < splitDigit.length(); j++)
                    sum += splitDigit[j] - '0';
                //mutex.unlock();
            }
            else
            {
                //mutex.lock();
                sum += doubleDigit;
                //mutex.unlock();
            }
        //}           
    }
}

void oddDigitSums(string card, int& sum)
{
    if ((card.length() - 1) % 2 == 0)
        card.pop_back();
    for (int i = card.length() - 1; i >= 0; i -= 2)
    {
        /*if (i % 2 != 0)
        {*/
            int digit = card[i] - '0';
            //mutex.lock();
            sum += digit;
            //mutex.unlock();
        //}       
    }
}

void luhnSingle(string card)
{
    bool cardLength;
    verifyLength(card, cardLength);
    if (!cardLength)
    {
        cout << "Invalid Length. Size " << card.length() << "\n";
        return;
    }

    string industryCard = "";
    getCardName(card[0], industryCard);

    auto start = std::chrono::high_resolution_clock::now();
    int singleDigitSum = 0;
    int oddDigitSum = 0; //algorithm works from
    for (int i = card.length() - 1; i >= 0; i--)
    {
        int digit = card[i] - '0'; //convert char to int
        
        if (i % 2 != 0) //check if the index is in a odd position rather than even
            oddDigitSum += digit;
        else
        {
            int doubleDigit = digit * 2;
            if (doubleDigit >= 10)
            { //converting the digit to string using an ostream
                string splitDigit = intToString(doubleDigit);
                //convertted it to string so it's easier to split and add the digits
                for (size_t j = 0; j < splitDigit.length(); j++)
                    singleDigitSum += splitDigit[j] - '0';
            }
            else
                singleDigitSum += doubleDigit;
        }
    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long milliseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000;
    //cout << singleDigitSum + oddDigitSum << std::endl;
    if ((singleDigitSum + oddDigitSum) % 10 == 0)
        cout << card << " is Valid." << " Card Type: " 
            << industryCard << " " << milliseconds << "ms\n";
    else
        cout << card << " is Invalid. " << " Card Type: " 
            << industryCard << " " << milliseconds << "ms\n";
}