use strict;

use Test::More tests => 1;

BEGIN {
    use_ok('Trasker::TTDB::Projects');
};

my $projects = Trasker::TTDB::Projects->new();

