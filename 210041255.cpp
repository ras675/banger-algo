#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

bool validateHeader(const vector<string>& headers) {
if (headers.empty() || headers[0].find("PID") != 0) {

        cerr << "Error: First column must be 'PID'" << endl;
        return false;
    }

    int numColumns = headers.size();
    if ((numColumns - 1) % 4 != 0) {
        cerr << "Error: Column count must follow (4n + 1) pattern." << endl;
        return false;
    }

    int sectionSize = (numColumns - 1) / 4; // Number of columns in each section

    // Validate sections
    for (int i = 1; i <= sectionSize; i++) {
        if (headers[i].find("Allocation_") != 0) {
            cerr << "Error: Expected 'Allocation_' in column " << i + 1 << endl;
            return false;
        }
    }
    for (int i = sectionSize + 1; i <= 2 * sectionSize; i++) {
        if (headers[i].find("Max_") != 0) {
            cerr << "Error: Expected 'Max_' in column " << i + 1 << endl;
            return false;
        }
    }
    for (int i = 2 * sectionSize + 1; i <= 3 * sectionSize; i++) {
        if (headers[i].find("Available_") != 0) {
            cerr << "Error: Expected 'Available_' in column " << i + 1 << endl;
            return false;
        }
    }
    for (int i = 3 * sectionSize + 1; i < numColumns; i++) {
        if (headers[i].find("Need_") != 0) {
            cerr << "Error: Expected 'Need_' in column " << i + 1 << endl;
            return false;
        }
    }

    return true;
}

int convertToInt(const string& value) {
    if (value == "-") {
        return 0;
    }
    return stoi(value);
}

void countRowsColumns(const string& filename, int& rowCount, int& colCount, vector<string>& headers, vector<vector<int> >& dataMatrix) {
    ifstream file(filename);
    string line;
    rowCount = 0;
    colCount = 0;

    if (!file.is_open()) {
        cerr << "Error: Could not open file." << endl;
        return;
    }

    // Read header row
    if (getline(file, line)) {
        stringstream ss(line);
        string value;

        while (getline(ss, value, '\t')) {  // Splitting by TAB
            headers.push_back(value);
        }

        colCount = headers.size();
    }

    // Read the data rows and store them in the 2D matrix (skipping the first column and the first row)
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<int> rowData;
        for (int i = 0; i < colCount; ++i) {
            getline(ss, value, '\t');
            // Skip the first column (PID) and start from the second column (index 1)
            if (i > 0) {
                rowData.push_back(convertToInt(value));  // Convert string to integer, treating '-' as 0
            }
        }
        if (!rowData.empty()) {
            dataMatrix.push_back(rowData);
        }
        rowCount++;
    }

    file.close();
}

void extractSubMatrix(const vector<vector<int> >& originalMatrix, vector<vector<int> >& subMatrix, int startCol, int endCol) {
    for (const auto& row : originalMatrix) {
        vector<int> subRow(row.begin() + startCol, row.begin() + endCol);
        subMatrix.push_back(subRow);
    }
}

int main() {
    string filename = "input0.csv";
    int rows, columns;
    vector<string> headers;
    vector<vector<int> > dataMatrix;
    vector<string> pidList;

    ifstream file(filename);
    string line;

    // Skip the header row


    // Reinitialize to count rows and columns
    countRowsColumns(filename, rows, columns, headers, dataMatrix);

   // cout << "\nRows (excluding header): " << rows << endl;
   // cout << "Columns: " << columns << endl;

    // Validate file format
    if (rows == 0 || (columns % 4 != 1 && columns > 1)) {
        cout << "Invalid file format!" << endl;
        return 1;  // Return error code
    }

    // Strip the first and last header characters
    string temp;
    temp = headers[0];
    headers[0] = temp.substr(1, temp.size());
    temp = headers[columns - 1];
    headers[columns - 1] = temp.substr(0, temp.size() - 1);

    if (!validateHeader(headers)) {
        cout << "Invalid file format!" << endl;
        return 1;
    }
    cout<<"Valid File Format"<<endl;

    /* Print header row
    cout << "Header Row:" << endl;
    for (const string& header : headers) {
        cout << header << endl;
    }*/

    if (getline(file, line)) {
        // Nothing needed here as we skip it
    }

    // Now read the PID values from the first column of each subsequent row
    while (getline(file, line)) {
        stringstream ss(line);
        string value;

        // Extract PID value (the first value in the row)
        if (getline(ss, value, '\t')) {
            pidList.push_back(value.substr(1,value.size())); // Store PID value in pidList
        }
    }

    // Print PID List (first column from second row to last)
    /*cout << "\nPID:" << endl;
    for (const auto& pid : pidList) {
        cout << pid << " ";
    }
    cout << endl;*/

    file.close();

    // Create three additional submatrices
    int sectionSize = (columns - 1) / 4;

    // Matrix 1 (Columns 2 to column/4+1)
    vector<vector<int> > Allocation;
    extractSubMatrix(dataMatrix, Allocation, 0, sectionSize);

    vector<int> sum;
    int i=0;
    /*int s=0;
    for (const auto& row : Allocation) {
        for (const auto& cell : row) {
            if(s==0){
                sum.push_back(cell);
            }else{
                sum[i]+=cell;
                i=(i+1)%(columns/4);
            }
        }s=1;
    }

    /*for(const auto &row: sum){
        cout<<row<<" ";
    }*/

    for (auto& row : Allocation) {
        row.push_back(0);
    }


    vector<vector<int> > Max;
    extractSubMatrix(dataMatrix, Max, sectionSize, 2 * sectionSize);

    // Matrix 2 (Columns column/4+2 to 2*column/4+1)
    vector<vector<int> > Matrix;
    vector<int> Available;
    extractSubMatrix(dataMatrix, Matrix, 2 * sectionSize, 3 * sectionSize);
    for (const auto& row : Matrix) {
        for (const auto& cell : row) {
            Available.push_back(cell);
        }
        break;
    }

    // Matrix 3 (Columns 2*column/4+2 to 3*column/4+1)
    vector<vector<int> > Need;
    extractSubMatrix(dataMatrix, Need, 3 * sectionSize, 4 * sectionSize);



    /*cout << "\nMax:" << endl;
    for (const auto& row : Max) {
        for (const auto& cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }

    cout << "\nAvailable:" << endl;
    for (const auto& row : Available) {
        cout << row << " ";
    }
    cout << endl;

    cout << "\nNeed:" << endl;
    for (const auto& row : Need) {
        for (const auto& cell : row) {
            cout << cell << " ";
        }
        cout << endl;
    }*/
    cout << endl << endl;
    vector<string> safeSequence;
    bool deadlock=false;
    int processServed = 0;
    columns=columns/4;
    int process=rows;
    int j;
    while (processServed < process)
    {
        int couldntServe = 0;

        for (i = 0; i < process; )
        {

            if (Allocation[i][columns] == 1)
            {
                i++;
                continue;
            }

            bool canServe = true;
            for (j = 0; j < columns; j++)
            {
                if (Available[j] < Max[i][j] - Allocation[i][j])
                {
                    canServe = false;
                    cout<<"Process "<<pidList[i]<<" can't be served due to less resources!!"<<endl<<endl;
                    break;
                }
            }

            if (canServe)
            {
                cout << "Process " << pidList[i] << " executed\n";
                safeSequence.push_back(pidList[i]);
                Allocation[i][columns] = 1;
                cout << "Available Resources:\n";
                for (j = 0; j < columns; j++)
                {
                    Available[j] += Allocation[i][j];
                    cout<<Available[j]<<" ";
                }
                processServed++;
                i=0;
                cout<<endl<<endl;
            }
            else
            {
                i++;
                couldntServe++;
            }
        }

        if (couldntServe == process - processServed)
        {
            deadlock=true;
            break;
        }
    }
    if(!deadlock)
    {
        cout<<"All process Served!!\n\nSafe Sequence:\n";
        for(const auto& row: safeSequence){
            cout<<row<<" ";
        }
    }
    else
    {
        cout<<"Deadlock detected!!";
    }

    return 0;
}
