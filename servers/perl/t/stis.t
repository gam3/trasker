use strict;

use Test::More tests => 2;

$Trasker::TTDB::DBI::dbi = DBI->connect('dbi:Mock:', '', '', { AutoCommit => 0 });

BEGIN {
    use_ok('Trasker::TTDB::DBI');
};
if ($@) {
    diag($@);
}

use Data::Dumper;
#diag(Dumper(Trasker::TTDB::DBI->get_sti('timeslice_init')));
ok(1);

