use strict;

use Test::More tests => 2;

use TTDB::DBI qw (get_dbh);

use_ok('TTDB::Auto');

our $dbh = get_dbh;

$dbh->do(qq/delete from auto/) or die;

$dbh->do(qq/SELECT setval('auto_id_seq', 1, false)/);

$dbh->commit;

TTDB::Auto->create(user_id => 1, project_id => 1, host => 'testme');

TTDB::Auto->create(user_id => 1, project_id => 1, name => 'testme');

TTDB::Auto->create(user_id => 1, project_id => 1, class => 'testme');

TTDB::Auto->create(user_id => 1, project_id => 1, role => 'testme');

TTDB::Auto->create(user_id => 1, project_id => 1, title => 'testme');

TTDB::Auto->create(user_id => 1, project_id => 1, desktop => 'testme');

TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
);

TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme'
);

TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme',
host => 'testme'
);

TTDB::Auto->create(user_id => 1, project_id => 1,
title => '% testme %',
);

my $a = TTDB::Auto->get(user_id => 1,
    host => 'testme',
    name => 'testme',
    class => 'testme',
    role => 'testme',
    desktop => 'testme',
    title => 'testme',
);

is($a->id, 9);

TTDB::Auto->create(user_id => 1, project_id => 1, host => '%.test.net');

