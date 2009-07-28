use strict;

use Test::More tests => 2;

use Trasker::TTDB::DBI qw (get_dbh);

use_ok('Trasker::TTDB::Auto');

our $dbh = get_dbh;

$dbh->do(qq/delete from auto/) or die;

$dbh->do(qq/SELECT setval('auto_id_seq', 1, false)/);

$dbh->commit;

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1, host => 'testme');

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1, name => 'testme');

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1, class => 'testme');

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1, role => 'testme');

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1, title => 'testme');

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1, desktop => 'testme');

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
);

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme'
);

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => 'testme',
role => 'testme',
name => 'testme',
class => 'testme',
desktop => 'testme',
host => 'testme'
);

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1,
title => '% testme %',
);

my $a = Trasker::TTDB::Auto->get(user_id => 1,
    host => 'testme',
    name => 'testme',
    class => 'testme',
    role => 'testme',
    desktop => 'testme',
    title => 'testme',
);

is($a->id, 9);

Trasker::TTDB::Auto->create(user_id => 1, project_id => 1, host => '%.test.net');

