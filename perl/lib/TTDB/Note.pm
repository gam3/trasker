use strict;

package TTDB::Note;

use TTDB::DBI qw (get_dbh);

use Params::Validate qw (validate);

sub new
{
    my $class = shift;
    my %p = validate(@_, {
        note => 1,
        type => {
	    default => 1,
	},
        time => {
	    optional => 1,
	    isa => "Date::Calc",
	},
        user => {
	    optional => 1,
	    isa => "TTDB::User",
	},
        project => {
	    optional => 1,
	    isa => "TTDB::Project",
	},
	user_id => 0,
	project_id => 0,
    });

    bless { %p }, $class;
}

sub get
{
    my $class = shift;
    my $dbh = get_dbh('read');
    my %p = validate(@_, {
        id => 1,
    });
    my @ret;
    my @args = ();

    my $sql = "select * from notes";

    $sql .= ' where id = ?';

    my $sth = $dbh->prepare($sql);

    $sth->execute($p{id});

    map({bless $_, 'TTDB::Note'} @{$sth->fetchall_arrayref({})});
}

sub create
{
    my $class = shift;

    my $dbh = get_dbh;

    my $self = $class->new(@_);

    my $st;

    my $st_id = $dbh->prepare(qq/select nextval('note_id_seq')/);

    my @extra = ();

    my $user_id = $self->user_id;
    my $project_id = $self->project_id;

    if (my $time = $self->time) {
	$st = $dbh->prepare("insert into notes (type, user_id, project_id, note, time) values (?, ?, ?, ?, ?)");
	push @extra, $time->mysql;
    } else {
	$st = $dbh->prepare("insert into notes (type, user_id, project_id, note, time) values (?, ?, ?, ?, now())");
    }

    eval {
	$st->execute($self->type, $user_id, $project_id, $self->note, @extra);
    }; if ($@) {
	$dbh->rollback;
	die $@;
    } else {
	$dbh->commit or die $dbh->errstr();
    }

    $self;
}

sub time
{
    my $self = shift;

    $self->{time};
}

sub type
{
    my $self = shift;

    $self->{type};
}

sub user_id
{
    my $self = shift;

    if (my $u = $self->{user}) {
        return $u->id;
    } else {
        return $self->{user_id};
    }
}

sub project_id
{
    my $self = shift;

    if (my $p = $self->{project}) {
        return $p->id;
    } else {
        return $self->{project_id};
    }
}

sub user
{
    my $self = shift;

    if ($self->user_id) {
	$self->{user} ||= TTDB::User->get(id => $self->user_id);
    }
}

sub project
{
    my $self = shift;

    require TTDB::Project;
    if ($self->project_id) {
	$self->{project} ||= TTDB::Project->get(id => $self->project_id);
    }
    $self->{project};
}

sub text
{
    my $self = shift;

    $self->{note};
}

1;
__END__

=head1 NAME

TTDB::Note - Perl interface to the tasker notes table

=head1 SYNOPSIS

  use TTDB::Note;

  $auto = TTDB::Note->new(user => $user);

  $auto = TTDB::Note->create(
      project_id => I<projectid>,
      user_id => I<userid>,
  );

  $auto = TTDB::Note->get(user => $user, role => 'bob'):

=head1 DESCRIPTION

Normally the I<note> is returned from the I<TTDB::Notes> interface or somewhere else.

=head2 Constructor

=over

=item new

This does not put the object into the database.

=item get

Get an object from the database.

=item create

This will create a auto entry, if one does not exist. 

=back

=head2 Methods

=over

=item  id

return the I<id> of the object.

=item  user_id

return the I<user_id> for the object.

=item  project_id

return the I<project_id> for the object.

=item delete

This will delete the object from the database.

=back

=head1 SEE ALSO

L<TTDB::Notes>

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

