WebAgent is a C based http client, robot & html parser and indexer
(using sql dbms, such as mysql as datastore) that aims to watch, index
and notify subscribers of relevant changes to any web page.

It has run for about 10 years on http://www.webagent.nl/  
(see http://web.archive.org/web/20090603071755/http://www.webagent.nl/?)
serving up to 10000 subscribers, before the hard drive crashed down and 
burned all subscription data for good.

For me the project served as an exercise in C, in which I wanted to 
implement at least:
* a http client (wrote it from scratch) 
* a html parser (wrote it using lex)
This was definitely a sign of the (my) times, and nowadays this
functionality is much more better and stable built in a few lines of
developer code in e.g. python 

The complete WebAgent project as checked into repository consists of this 
http client and html parser, but also a newsletter / mail application 
(to mail out the detected changes) and a complete website (in PHP 3 I 
believe...) for endusers to actually subscribe to their provided urls 
and configure their filters and settings.

Please bare, the PHP code is ancient. The C code is of an amateur. 
Nevertheless, the thing worked pretty stable, was very fast, and served 
a decent sized base of almost 10K subscribers for almost 10 years.
