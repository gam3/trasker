use strict;

use Test::More tests => 1;

use TTDB::DBI qw (get_dbh);

use_ok('TTDB::Auto');

our $dbh = get_dbh;

$dbh->do(qq/delete from auto/) or die;
$dbh->do(qq/SELECT setval('auto_id_seq', 1, false)/);

our $x = $dbh->prepare(<<SQL) or die;
insert into auto 
        (host, name, class, role, title, desktop, presidence)
 values (   ?,    ?,     ?,    ?,     ?,       ?,          ?)
SQL

$x->execute(('%') x 6,  0);
$x->execute(('%') x 6,  1000);

$dbh->commit;

