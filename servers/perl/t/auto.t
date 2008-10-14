use strict;

use Test::More tests => 4;

BEGIN {
    use_ok('TTDB::Auto');
    use_ok('TTDB::User');
};

eval {
    TTDB::Auto->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

my $auto;

$auto = TTDB::Auto->create(
    user_id => 1,
    project_id => 1,
    name => 'testme',
    class => 'testme',
    role => 'testme',
    title => 'This is a title.',
    desktop => 'testme',
    host => 'harpo',
);

my $nauto = TTDB::Auto->get(
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

$auto = TTDB::Auto->get(
    user_id => 1,
    name => 'testme',
    class => 'testme',
    role => 'testme',
    title => 'This is a title %',
    desktop => 'testme',
    host => 'harpo',
);

$auto->delete();

