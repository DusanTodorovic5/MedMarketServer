# Medicine Market Server
Native TCP, Rest and Websocket server with dedicated client, all written in C++

All versions of project work the same way. 
Custom authentication is implemented in C++, currently passwords are hashed using std::hasher which is not the safest method to do it(or safe at all), but it is fairly easy and straight-forward to replace the hash function with custom library. 
User has ability to send query for given medicine, to buy it, select it or to authenticate. The types of messages that user can send are:

 - ``login;username;password``
 - ``register;username;password``
 - ``update;username;old_password;new_password``
 - query, which i will explain in depth
 
## Query

Medicine Market features SQL-Like query syntax where user can specify filter, examples are:

 - ``select for Alex where effectiveness = 3 and boxes < 5``
 - ``buy for Dusan where price >= 5 and purpose = Flu and boxes = 2``
 - ``buy anyways for Doctor where name != K21 and name != M21 and price <= 20 and price >= 12 and boxes = 4``
 
User can specify if he wants to get info(typing ``select``) or buy medicine(typing ``buy`` or ``buy anyways``). Difference between mods of buying are that if user specifies multiples queryes, with ``buy``, he will buy them only if all of them are avaliable, where with ``buy anyways`` he will buy those that are avaliable.
User can filter by:

 - name
 - boxes(filterByAvailability)
 - price
 - purpose
 - effectiveness

## Native TCP

Native TCP uses wrapper around posix tcp sockets. If you wish to run it, you should just write small wrapper around given methods.

## WebSocket

WebSocket uses [websocketpp](https://github.com/zaphoyd/websocketpp)

## Rest Api

Features custom tokenizer with timeout of 30 seconds, refreshed once a new query is sent.
Possible endpoints are
 - login[POST] - with headers Username and Password - returns unique token
 - register[POST] - with headers Username and Password - returns if successfull
 - select[GET] - header with token and with query given as parameters for example ``select/name=Q21&price=20&priceOper=<``
     Oper is argument specifing type of operand for price, in this case, price < 20
 - buy[POST] - header with token and body with full query like with sockets, for example ``name = Q21 and price < 20 and boxes = 3``
 - buyAnyways[POST] - header with token and body full query like sockets, for example ``name = Q21 and price < 20 and boxes = 3``
