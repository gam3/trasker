use strict;

package TTDB::Users;

use TTDB::DBI qw (get_dbh);

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

our $dir = "/home/gam3/.tasker/";

sub new
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
        project_id => 0,
    });

    my $sth;
    if (my $pid = $p{project_id}) {
	$sth = $dbh->prepare(<<SQL) or die;
select users.name as user_name,
       users.fullname as user_fullname,
       users.id as user_id,
       'eof' as eof
  from users, user_project
 where user_project.user_id = user.id
   and user_project.project_id = ?
SQL

	$sth->execute($pid);
    } else {
	$sth = $dbh->prepare(<<SQL) or die;
select users.name as user_name,
       users.fullname as user_fullname,
       users.id as user_id,
       'eof' as eof
  from users
SQL

	$sth->execute();
    }

    my @data;
    while (my $data = $sth->fetchrow_hashref()) {
	push(@data, bless { data => $data, id => $data->{user_id} }, 'TTDB::User');
    }

    return bless { data => \@data }, $class;
}

sub entries
{
    my $self = shift;

    @{$self->{data}};
}

1;
__END__

