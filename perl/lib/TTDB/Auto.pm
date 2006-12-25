use strict;

package TTDB::Auto;

use TTDB::DBI qw (get_dbh);

use TTDB::User;
use TTDB::Project;

use Params::Validate;

use Carp qw (croak);

sub new
{
    my $class = shift;
    my %p = validate(@_, {
	project_id => 0,
	project => 0,
	user_id => 0,
	user => 0,
	host => 0,
	name => 0,
	class => 0,
	role => 0,
	title => 0,
	desktop => 0,
    });

    croak "Need at least one argument" unless keys %p;
    croak "Can't have project and project_id"  if ( $p{project} && $p{project_id} );
    croak "Can't have user and user_id" if ( $p{user} && $p{user_id} );

    if (my $prj = $p{project}) {
        $p{project_id} = $prj->id;
    }
    if (my $user = $p{project}) {
        $p{user_id} = $user->id;
    }
    croak "Need a project." unless $p{project_id};
    my $pr = 0;

    if ($p{name}) {
       $pr |= 32;
    }
    if ($p{class}) {
       $pr |= 16;
    }
    if ($p{role}) {
       $pr |= 8;
    }
    if ($p{title}) {
       $pr |= 4;
    }
    if ($p{desktop}) {
       $pr |= 2;
    }
    if ($p{host}) {
       $pr |= 1;
    }

#    $pr -= scalar keys %p;

    if ($pr == 5 && $p{desktop}) {
#        $pr++;
    }

    $p{presidence} = $pr;

    return bless { %p }, $class;
}

sub id
{
    shift->{id} or die 'No id';
}

sub project_id
{
    my $self = shift;

    $self->{project_id};
}

sub delete
{
    my $self = shift;

    my $dbh = get_dbh('commit');

    my $id = $self->id();

    my $sth = $dbh->prepare('delete from auto where id = ?');

    eval {
	$sth->execute($id);
    }; if ($@) {
	$dbh->rollback();
    } else {
	$dbh->commit();
    }
}

sub create
{
    my $self = shift;

    die if $self->{id};

    my $dbh = get_dbh;

    my $sth_id;
    if (0) {
	$sth_id = $dbh->prepare('select LAST_INSERT_ID()');
    } else {
	$sth_id = $dbh->prepare("select currval('auto_id_seq')");
    }
    my $sth = $dbh->prepare(<<SQL);
insert into auto (project_id, user_id, host, name, class, role, title, desktop, presidence)
          values (         ?,       ?,    ?,    ?,     ?,    ?,     ?,       ?,          ?)
SQL

    $sth->execute(
	$self->{project_id},
	$self->{user_id},
	$self->{host} || '%',
	$self->{name} || '%',
	$self->{class} || '%',
	$self->{role} || '%',
	$self->{title} || '%',
	$self->{desktop} || '%',
	$self->{presidence}
    );
    $sth_id->execute();
    my $id = $sth_id->fetchrow_array();
    $self->{id} = $id;

    $dbh->commit();

    $self;
}

sub get
{
    my $dbh = get_dbh;
    my $class = shift;

    my %p = validate(@_, {
	user_id => 0,
	user => 0,
	host => 1,
	name => 1,
	class => 1,
	role => 1,
	title => 1,
	desktop => 1,
    });

    croak(q(Can't have both user and user_id)) if (defined $p{user} && defined $p{user_id});
    croak(q(Must have an user or user_id)) if (!defined $p{user} && !defined $p{user_id});

    if (my $user = $p{user}) {
        $p{user_id} = $user->id;
    }

    my $sth;

    my $id = $p{id};
	$sth = $dbh->prepare(<<SQL);
select * 
  from auto
 where user_id = ?
   and ? like host
   and ? like name
   and ? like class
   and ? like role
   and ? like title
   and ? like desktop
   order by presidence desc
   limit 1
SQL

    $sth->execute(
	$p{user_id},
	$p{host} || '',
	$p{name} || '',
	$p{class} || '',
	$p{role} || '',
	$p{title} || '',
	$p{desktop} || ''
    );

    my $data = $sth->fetchrow_hashref();

    unless ($data) {
        return undef;
    }

    return bless { %$data }, $class;
}

1;
__END__

=head1 NAME

TTDB::Auto - Perl interface to the tasker auto table

=head1 SYNOPSIS

  use TTDB::Auto;

  $auto = TTDB::Auto->new(user => $user):

  $auto->create();

  $auto = TTDB::Auto->get(user => $user, role => 'bob'):

=head2 Constructor

=over

=item new

This does not put the obkject into the database.

=item get

Get an object from the database.

=back

=head2 Methods

=over

=item  id

=item  user_id

=item  project_id

=item create

This will create a auto entry, if one does not exist. 

=item delete

This will delete a auto entry.

=back

=head1 DESCRIPTION

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

