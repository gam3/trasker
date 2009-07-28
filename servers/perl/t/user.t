use strict;

use Test::More tests => 9;

BEGIN {
    use_ok('Trasker::TTDB::User');
};

eval {
    Trasker::TTDB::User->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

our $user_A = Trasker::TTDB::User->get(user => 'bob');
our $user_B = Trasker::TTDB::User->get(id => $user_A->id());

$user_A->set_current_project(project_id => 1, host => 'none');

is( $user_A->id, $user_B->id, 'compare ids');
is($user_A->id, 1, 'test id');
is( $user_A->name, $user_B->name, 'compare names' );
is($user_A->name, 'bob', 'test names');
is( $user_A->fullname, $user_B->fullname, 'compare fullname');
is( $user_A->fullname, 'Bob Johnson', 'test fullname');

$user_A->add_note(note => "This is a user note");

sleep 1;

$user_A->set_current_project(project_id => 2, host => 'none');

pass('all');

my $project = $user_A->project(name => 'First');

my $note = $project->add_note(
    note => 'This is a note',
    type => 2,
);

#use Data::Dumper;
#diag Dumper $note;
