use strict;

use Test::More tests => 4;
use Test::Exception;

BEGIN {
    use_ok('Tasker::TTDB::Note');
};

use Tasker::TTDB::User;
use Tasker::TTDB::Project;

our $user = Tasker::TTDB::User->get(user => 'bob');
our $proj = Tasker::TTDB::Project->get(id => 1);

my $x = Tasker::TTDB::Note->create(
    user => $user,
    project => $proj,
    note => "This is a note",
);

my $y = Tasker::TTDB::Note->get(
    id => 1,
);

use Data::Dumper;
print Dumper $x, $y;

pass('two');
pass('three');
pass('four');
