use strict;

use Test::More tests => 4;
use Test::Exception;

BEGIN {
    use_ok('Trasker::TTDB::Note');
};

use Trasker::TTDB::User;
use Trasker::TTDB::Project;

our $user = Trasker::TTDB::User->get(user => 'bob');
our $proj = Trasker::TTDB::Project->get(id => 1);

my $x = Trasker::TTDB::Note->create(
    user => $user,
    project => $proj,
    note => "This is a note",
);

my $y = Trasker::TTDB::Note->get(
    id => 1,
);

use Data::Dumper;
print Dumper $x, $y;

pass('two');
pass('three');
pass('four');
