use strict;

package TTDB::Users;
use TTDB::User;

use TTDB::DBI qw (get_dbh);
use TTDB::Users;

use Params::Validate qw( validate validate_pos SCALAR BOOLEAN HASHREF OBJECT );

our $dir = "/home/gam3/.tasker/";

sub new
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
#        project_id => 0,
        project => 0,
    });

    my $pid = $p{project_id};
    my $project = $p{project};
    if ($project) {
	$pid = $project->id;
    }

    my $sth;
    if (my $pid = $p{project_id}) {
	$sth = $dbh->prepare(<<SQL);
select users.name as user_name,
       users.fullname as user_fullname,
       users.id as user_id,
       'eof' as eof
  from users, user_project
 where user_project.user_id = users.id
   and user_project.project_id = ?
SQL
print $sth->{Statment};
	$sth->execute($pid);
    } else {
	$sth = $dbh->prepare(<<SQL);
select users.name as user_name,
       users.fullname as user_fullname,
       users.id as user_id,
       'eof' as eof
  from users
SQL

	$sth->execute();
    }

    my @data;
    require TTDB::UserProject;
    while (my $data = $sth->fetchrow_hashref()) {
	my $user = TTDB::User->new(id => $data->{user_id}, name => $data->{user_name}, fullname => $data->{user_fullname}  );
        if ($project) {
	    push(@data,  TTDB::UserProject->new(user => $user, project => $project));
	} else {
	    push(@data, $user);
	}
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

