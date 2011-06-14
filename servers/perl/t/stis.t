use strict;

use Test::More tests => 2;

$Trasker::TTDB::DBI::dbi = DBI->connect('dbi:Mock:', '', '', { AutoCommit => 0 });

BEGIN {
    use_ok('Trasker::TTDB::DBI');
};
if ($@) {
    diag($@);
}

$Trasker::TTDB::DBI::db = 'sqlite3';
Trasker::TTDB::DBI->_set_stis();
$Trasker::TTDB::DBI::db = 'pgsql';
Trasker::TTDB::DBI->_set_stis();
$Trasker::TTDB::DBI::db = 'mysql';
Trasker::TTDB::DBI->_set_stis();

use Data::Dumper;
diag(Dumper(Trasker::TTDB::DBI->get_sti('timeslice_init')));
ok(1);

