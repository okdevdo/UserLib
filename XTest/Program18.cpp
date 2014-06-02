/******************************************************************************
    
	This file is part of XTest, which is part of UserLib.

    Copyright (C) 1995-2014  Oliver Kreis (okdev10@arcor.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by 
	the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/
#include "Program.h"
#include "AsyncIOManager.h"
#include "AsyncTCPClient.h"
#include "ScopedLock.h"

static CConstPointer pSQLite3Statement[] = {
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client01.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client02.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client03.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client04.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client05.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client06.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client07.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client08.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client09.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client0A.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client0B.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client0C.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client0D.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client0E.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client0F.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client10.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client11.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client12.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client13.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client14.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client15.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client16.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client17.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client18.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client19.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client1A.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client1B.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client1C.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client1D.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client1E.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client1F.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
	,
	_T("CONNECT: OKWIN8\\Oliver ok99live C:\\Users\\Oliver\\Documents\\client20.sqlite\n")
	_T("STATEMENT: ") _T("DROP TABLE IF EXISTS okTest;")
	_T("CREATE TABLE okTest(")
	_T("EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,")
	_T("EmployeeName TEXT(100) NOT NULL,")
	_T("EmployeeSalary FLOAT NOT NULL,")
	_T("Comment TEXT(500) NULL")
	_T(");\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis', 100.0, 'TestComment');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis1', 200.0, 'TestComment1');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis2', 300.0, 'TestComment2');\n")
	_T("STATEMENT: ") _T("INSERT INTO okTest (EmployeeName, EmployeeSalary, Comment) VALUES ('okreis3', 400.0, 'TestComment3');\n")
	_T("STATEMENT: ") _T("SELECT * FROM okTest;\n")
	_T("DISCONNECT: CLOSE\n")
};

class CServerResponse : public CCppObject
{
public:
	CServerResponse(CConstPointer crequest = NULL): _crequest(__FILE__LINE__ crequest) 
	{
		append_slash();
	}
	CServerResponse(ConstRef(CStringBuffer) crequest) : _crequest(crequest) 
	{
		append_slash();
	}
	virtual ~CServerResponse() {}

	__inline ConstRef(CStringBuffer) get_ClientRequest() const { return _crequest; }
	__inline ConstRef(CStringBuffer) get_ServerResponse() const { return _sresponse; }
	__inline void set_ServerResponse(ConstRef(CStringBuffer) v) { _sresponse = v; }
	__inline void set_ServerResponse(CConstPointer v) { _sresponse.SetString(__FILE__LINE__ v); }

	void Print()
	{
		_tprintf(_T("%s"), _crequest.GetString());
		if (_sresponse.IsEmpty())
			_tprintf(_T("(null)\n"));
		else
			_tprintf(_T("%s"), _sresponse.GetString());
	}

protected:
	void append_slash()
	{
		CStringConstIterator it(_crequest);

		if (it[it.GetLength() - 1] != _T('\n'))
			_crequest.AppendString(_T("\n"));
	}

	CStringBuffer _crequest;
	CStringBuffer _sresponse;
};

static void __stdcall CServerResponsesDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CServerResponse) pResponse = CastAnyPtr(CServerResponse, CastMutable(Pointer, data));

	pResponse->release();
}

class CServerResponses : public CDataDoubleLinkedListT<CServerResponse>
{
public:
	CServerResponses(CConstPointer stmts = NULL) : CDataDoubleLinkedListT<CServerResponse>(__FILE__LINE__0)
	{
		if (stmts)
			split_and_append(stmts);
	}
	~CServerResponses() {}

	void split_and_append(CConstPointer stmts)
	{
		CStringBuffer sStmts(__FILE__LINE__ stmts);
		CPointer pStmt[64];
		dword outputsize = 0;

		sStmts.Split(_T("\n"), pStmt, 64, &outputsize);
		for (dword i = 0; i < outputsize; ++i)
		{
			if (pStmt[i])
				Append(OK_NEW_OPERATOR CServerResponse(pStmt[i]));
		}
	}
};

class CSqLite3ServiceClient : public CAsyncTCPClient
{
public:

	CSqLite3ServiceClient(Ptr(CAsyncIOManager) pManager = NULL, CConstPointer stmts = NULL) :
		CAsyncTCPClient(pManager), _responses(stmts), _current_response(_responses.Begin()), _currentRowCnt(0), _currentRow(0), _bQuit(false)
	{
	}

	CSqLite3ServiceClient(Ptr(CAsyncIOData) pData) :
		CAsyncTCPClient(), _responses(), _current_response(_responses.Begin()), _currentRowCnt(0), _currentRow(0), _bQuit(false)
	{
		m_pData = pData;
	}

	virtual ~CSqLite3ServiceClient() {}

	__inline bool get_Quit() const { return _bQuit; }

	void Read(Ptr(CAbstractThreadCallback) pHandler)
	{
		CByteBuffer buf(__FILE__LINE__ 8192);

		CAsyncTCPClient::Read(buf, pHandler);
	}

	void Write(Ptr(CAbstractThreadCallback) pHandler)
	{
		CStringBuffer tmp((*_current_response)->get_ClientRequest());
		CByteBuffer buf;

		tmp.convertToUTF8(buf, false);
		CAsyncTCPClient::Write(buf, pHandler);
	}

	void process_server_response()
	{
		CStringBuffer tmp;
		CPointer output[64] = { NULL };
		dword outputsize = 0;

		tmp.convertFromUTF8(GetData()->get_buffer(), false);
		(*_current_response)->set_ServerResponse(tmp);
		tmp.Split(_T("\n"), output, 16, &outputsize);
		for (dword i = 0; i < outputsize; ++i)
		{
			if (s_strnicmp(output[i], _T("CONNECT:"), 8) == 0)
			{
				CStringBuffer tmp1(__FILE__LINE__ output[i] + 8);
				CPointer output1[16] = { NULL };
				dword output1size = 0;
				CStringBuffer errCode;
				CStringBuffer errMsg;
				dword state = 0;

				tmp1.SplitQuoted(_T("\""), _T(" "), output1, 16, &output1size);
				for (dword i = 0; i < output1size; i++)
				{
					if (!(StrEmpty(output1[i])))
					{
						switch (state)
						{
						case 0:
							errCode.SetString(__FILE__LINE__ output1[i]);
							++state;
							break;
						case 1:
							errMsg.SetString(__FILE__LINE__ output1[i]);
							++state;
							break;
						default:
							break;
						}
					}
				}
				if (errCode.Compare(_T("GRANTED"), 0, CStringLiteral::cIgnoreCase) == 0)
					++_current_response;
			}
			else if (s_strnicmp(output[i], _T("RESULT:"), 7) == 0)
			{
				CStringBuffer tmp1(__FILE__LINE__ output[i] + 7);
				CPointer output1[16] = { NULL };
				dword output1size = 0;
				CStringBuffer sColumns;
				word vColumns = 0;
				CStringBuffer sRows;
				dword state = 0;

				tmp1.SplitQuoted(_T("\""), _T(" "), output1, 16, &output1size);
				for (dword i = 0; i < output1size; i++)
				{
					if (!(StrEmpty(output1[i])))
					{
						switch (state)
						{
						case 0:
							sColumns.SetString(__FILE__LINE__ output1[i]);
							++state;
							break;
						case 1:
							sRows.SetString(__FILE__LINE__ output1[i]);
							++state;
							break;
						default:
							break;
						}
					}
				}
				sColumns.Split(_T("="), output1, 16, &output1size);
				vColumns = Castword(s_strtoul(output1[1], NULL, 10));
				if (vColumns > 0)
				{
					sRows.Split(_T("="), output1, 16, &output1size);
					_currentRowCnt = s_strtoll(output1[1], NULL, 10);
					_currentRow = 0;
					_responses.InsertAfter(_current_response, OK_NEW_OPERATOR CServerResponse(_T("FETCHROWS: ROW=0\n")));
					++_currentRow;
				}
				++_current_response;
			}
			else if (s_strnicmp(output[i], _T("ROWDATA:"), 8) == 0)
			{
				CStringBuffer tmp1;

				if (_currentRow < _currentRowCnt)
				{
					tmp1.FormatString(__FILE__LINE__ _T("FETCHROWS: ROW=%lld\n"), _currentRow);
					_responses.InsertAfter(_current_response, OK_NEW_OPERATOR CServerResponse(tmp1));
					++_currentRow;
				}
				++_current_response;
			}
			else if (s_strnicmp(output[i], _T("DISCONNECT:"), 11) == 0)
			{
				CStringBuffer tmp1(__FILE__LINE__ output[i] + 11);

				tmp1.Trim();
				if (tmp1.Compare(_T("QUIT"), 0, CStringLiteral::cIgnoreCase) == 0)
				{
					_bQuit = true;
					return;
				}
			}
		}
	}

	void Print()
	{
		CServerResponses::Iterator it = _responses.Begin();

		while (it)
		{
			(*it)->Print();
			++it;
		}
	}

protected:
	CServerResponses _responses;
	CServerResponses::Iterator _current_response;
	sqword _currentRowCnt;
	sqword _currentRow;
	bool _bQuit;
};

class AsyncTCPClientListEqualFunctor
{
public:
	bool operator()(ConstPtr(CAsyncTCPClient) r1, ConstPtr(CAsyncTCPClient) r2) const
	{
		return r1->GetData() == r2->GetData();
	}
};

static sword __stdcall AsyncTCPClientListSearchAndSortFunc(ConstPointer item, ConstPointer data)
{
	Ptr(CSqLite3ServiceClient) pClient = CastAnyPtr(CSqLite3ServiceClient, CastMutable(Pointer, item));
	Ptr(CAsyncIOData) pData = CastAnyPtr(CAsyncIOData, CastMutable(Pointer, data));

	if (pClient->GetData() == pData)
		return 0;
	return 1;
}

static void __stdcall AsyncTCPClientListDeleteFunc(ConstPointer data, Pointer context)
{
	Ptr(CSqLite3ServiceClient) pClient = CastAnyPtr(CSqLite3ServiceClient, CastMutable(Pointer, data));

	pClient->release();
}

typedef CDataDoubleLinkedListT<CSqLite3ServiceClient> CAsyncTCPClientList;

class CSqLite3ServiceAsyncClass
{
public:
	CSqLite3ServiceAsyncClass() :
		io_manager(),
		clientlist(__FILE__LINE__0),
		svclientlist(__FILE__LINE__0)
	{
	}

	~CSqLite3ServiceAsyncClass()
	{
	}

	void close_client(Ref(CAsyncTCPClientList::Iterator) it)
	{
		(*it)->Close();
		(*it)->addRef();
		svclientlist.Append(*it);
		clientlist.Remove(it);
		if (clientlist.Count() == 0)
			io_manager.Stop();
	}

	dword read_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CSqLite3ServiceClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if (it)
		{
			Ptr(CSqLite3ServiceClient) pClient = *it;

			if ((pData->get_bytestransferred() == 0) || (pData->get_errorcode() != 0))
			{
				close_client(it);
				return 1;
			}
			pClient->process_server_response();
			if (pClient->get_Quit())
			{
				close_client(it);
				return 0;
			}
			try
			{
				pClient->Write(OK_NEW_OPERATOR CAsyncIODataCallback<CSqLite3ServiceAsyncClass>(this, &CSqLite3ServiceAsyncClass::write_callback, pClient->GetData()));
			}
			catch (CBaseException*)
			{
				close_client(it);
				return 1;
			}
		}
		return 0;
	}

	dword write_callback(Ptr(CAsyncIOData) pData)
	{
		CScopedLock lock;
		CSqLite3ServiceClient client(pData);
		CAsyncTCPClientList::Iterator it = clientlist.Find<AsyncTCPClientListEqualFunctor>(&client);

		if (it)
		{
			Ptr(CSqLite3ServiceClient) pClient = *it;

			if (pData->get_errorcode() != 0)
			{
				close_client(it);
				return 1;
			}

			try
			{
				pClient->Read(OK_NEW_OPERATOR CAsyncIODataCallback<CSqLite3ServiceAsyncClass>(this, &CSqLite3ServiceAsyncClass::read_callback, pClient->GetData()));
			}
			catch (CBaseException*)
			{
				close_client(it);
				return 1;
			}
		}
		return 0;
	}

	void Run()
	{
		CScopedLock lock;

		io_manager.Create();
		for (dword i = 0; i < (sizeof(pSQLite3Statement) / sizeof(CConstPointer)); ++i)
		{
			Ptr(CSqLite3ServiceClient) pClient = OK_NEW_OPERATOR CSqLite3ServiceClient(&io_manager, pSQLite3Statement[i]);

			pClient->Open(_T("localhost"), _T("6001"));
			clientlist.Append(pClient);
			pClient->Write(OK_NEW_OPERATOR CAsyncIODataCallback<CSqLite3ServiceAsyncClass>(this, &CSqLite3ServiceAsyncClass::write_callback, pClient->GetData()));
		}
		lock.unlock();
		io_manager.WaitForComplete();
		io_manager.Close();

		CAsyncTCPClientList::Iterator it = svclientlist.Begin();

		while (it)
		{
			(*it)->Print();
			++it;
		}
	}

protected:
	CAsyncIOManager io_manager;
	CAsyncTCPClientList clientlist;
	CAsyncTCPClientList svclientlist;
};

void TestSqLite3ServiceClient()
{
	CSqLite3ServiceAsyncClass aclass;

	aclass.Run();
}

