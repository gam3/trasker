use strict;

use Test::More tests => 1;

BEGIN {
    use_ok('Trasker::TTDB::Projects');
};

$Trasker::TTDB::DBI::dbi = DBI->connect('dbi:Mock:', '', '', { AutoCommit => 0 });

my $projects = Trasker::TTDB::Projects->new();

