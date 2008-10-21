use strict;

use Test::More tests => 4;

BEGIN {
    use_ok('Tasker::TTDB::Auto');
    use_ok('Tasker::TTDB::User');
};

eval {
    Tasker::TTDB::Auto->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

my $auto;

$auto = Tasker::TTDB::Auto->create(
    user_id => 1,
    project_id => 1,
    name => 'testme',
    class => 'testme',
    role => 'testme',
    title => 'This is a title.',
    desktop => 'testme',
    host => 'harpo',
);

my $nauto = Tasker::TTDB::Auto->get(
    user_id => 1,
    name => 'testme',
    class => 'testme',
    role => 'testme',
    title => 'This is a title.',
    desktop => 'testme',
    host => 'harpo',
);

is($nauto->id, $auto->id, 'get');

$auto->delete();

$auto = Tasker::TTDB::Auto->get(
    user_id => 1,
    name => 'testme',
    class => 'testme',
    role => 'testme',
    title => 'This is a title %',
    desktop => 'testme',
    host => 'harpo',
);

$auto->delete();

