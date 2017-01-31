/****************************************************************
   PROGRAM:   query.cc
   AUTHOR:    Drew Stefanik
   LOGON ID:  z1753912
   DUE DATE:  04/22/2016

   PURPOSE:   queries a postgresql databse based on user input

   INPUT:     standard input

   OUTPUT:    standard output, standard error output

   NOTES:     none
****************************************************************/
#include <iostream>     //cin, cout, cerr, endl, left
#include <iomanip>		//setw
#include <cstdlib>      //exit
#include <string>       //string
#include <postgresql/libpq-fe.h>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::left;
using std::setw;
using std::exit;
using std::string;

//function prototypes
PGresult * query_SQL(PGconn *, string);
string escape_string(PGconn *, const string &);

int main()
{
        //connect to the database
        PGconn * mydb;

        mydb = PQconnectdb("host=courses user=z1753912 dbname=z1753912 password=REDACTED");

        //error and quit if cannot connect to database
        if(PQstatus(mydb) != CONNECTION_OK)
        {
                cerr << "ERROR: Cannot connect to database" << endl;
                cerr << PQerrorMessage(mydb) << endl;
                exit(-1);
        }

        cout << "Connected to database" << endl;

	//declare variables
	string query, council, troop;
	PGresult * result;

	//request user input
	cout << "Council Name? ";
	cin >> council;
	cout << "Troop Number? ";
	cin >> troop;

	//build the required query as a string from SQL and user input
	//NOTE: results are the total of a troop's shop sales and its members individual sales
	query = "SELECT SS.cookie_name, SUM (SS.quantity + I.quantity), SUM (SS.quantity * SF.price + I.quantity * SF.price) \
		FROM z1753912.shop_sales SS, z1753912.sells_for SF, z1753912.belongs_to BT, z1753912.individual_sales I WHERE SS.council_name = '"
		+ council
		+ "' AND SS.troop_number = '" + troop
		+ "' AND BT.council_name = SS.council_name AND BT.troop_number = SS.troop_number AND BT.member_name = I.member_name AND I.cookie_name = SS.cookie_name"
		+ " AND SF.cookie_name = SS.cookie_name AND SF.council_name = SS.council_name"
		+ " GROUP BY SS.cookie_name;";

	result = query_SQL(mydb, query);

	//declare output variables
	int nrow, ncol;
	nrow = PQntuples(result);
	ncol = PQnfields(result);

	//if empty results, print message to that effect
	if(nrow == 0)
	{
		cout << endl << "No results for Council: " << council << " and Troop Number: " << troop << endl;
	}
	else
	{
		//print header
		cout << endl << "Results for Council: " << council << " and Troop Number: " << troop << endl;
		cout << setw(30) << left << "Cookie" << setw(25) << "Total Quantity Sold" << setw(25) << "Total Money Earned" <<  endl << endl;
	}
	//print each row and column of the query results
	for(int row = 0; row < nrow; row++)
	{
		for(int col = 0; col < ncol; col++)
		{
			if(col == 0)
			{
				cout << setw(30) << left << PQgetvalue(result, row, col) << " ";
			}
			else
			{
				cout << setw(25) << PQgetvalue(result, row, col) << " ";
			}
		}
		cout << endl;
	}
	cout << endl;

	//clear the results
	PQclear(result);

        //close the database connection
        PQfinish(mydb);
        cout << "Database closed" << endl;

        return 0;
}

/****************************************************************
   FUNCTION:   PGresult * insert_SQL(PGconn *, string)

   ARGUMENTS:  PGconn * conn, string s

   RETURNS:    PGresult *

   NOTES:      sends the query s to the database indicated by
		conn; if the query fails prints an error and
		exits; returns the results of the query
****************************************************************/
PGresult * query_SQL(PGconn * conn, string s)
{
        //send the query to the database
        PGresult * result;
        result = PQexec(conn, s.c_str());

        //check if the query was successful
        //print an error and exit if it failed
        if(PQresultStatus(result) != PGRES_TUPLES_OK)
        {
                cerr << "ERROR: Cannot execute query" << endl << PQresultErrorMessage(result);
                PQfinish(conn);
                cout << "Database closed" << endl;
                exit (-1);
        }

	return result;
}

/****************************************************************
   FUNCTION:   string escape_string(PGconn *, const string &)

   ARGUMENTS:  PGconn * conn, const string & s

   RETURNS:    string

   NOTES:      returns an escaped string that is suitable for
                use in SQL based on the input string s
****************************************************************/
string escape_string(PGconn * conn, const string & s)
{
        //declare variables
        string result;
        char * clean;

        //allocate appropriate size to the clean string
        clean = new char [s.length() * 2 + 1];

        //call postgres function to clean the string
        PQescapeStringConn(conn, clean, s.c_str(), s.length(), 0);

        result = clean;
        delete [] clean;
        return result;
}
