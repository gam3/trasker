use strict;

use Test::More tests => 3;

BEGIN {
    use_ok('TTDB::User');
};

use TTDB::DBI qw (get_dbh);

our $dbh = get_dbh;

$dbh->do(qq/delete from notes/) or die;
$dbh->do(qq/SELECT setval('notes_id_seq', 1, false)/);
$dbh->commit;

our $user_A = TTDB::User->get(user => 'bob');

$user_A->add_note(note => "This is a user note.");
pass('add note');

$user_A->add_note(note => "This is another user note.");
pass('add another note');

