use strict;

use Test::More tests => 4;

BEGIN {
    use_ok('Trasker::TTDB::Auto');
    use_ok('Trasker::TTDB::User');
};

eval {
    Trasker::TTDB::Auto->new();
};
if ($@) {
    pass('no args');
} else {
    fail('no args');
}

my $auto;

$auto = Trasker::TTDB::Auto->create(
    user_id => 1,
    project_id => 1,
    name => 'testme',
    class => 'testme',
    role => 'testme',
    title => 'This is a title.',
    desktop => 'testme',
    host => 'harpo',
);

my $nauto = Trasker::TTDB::Auto->get(
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

$auto = Trasker::TTDB::Auto->get(
    user_id => 1,
    name => 'testme',
    class => 'testme',
    role => 'testme',
    title => 'This is a title %',
    desktop => 'testme',
    host => 'harpo',
);

$auto->delete();

