use strict;

## @file
# This is the Note object

## @class
# The Note object
#
# This object holds a Note that the user entered.
#
package Trasker::TTDB::Note;

use Trasker::TTDB::DBI qw (get_dbh);

use Params::Validate qw (validate);

use Trasker::Date;

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
	},
        user => {
	    optional => 1,
	    isa => "Trasker::TTDB::User",
	},
        project => {
	    optional => 1,
	    isa => "Trasker::TTDB::Project",
	},
	user_id => 0,
	project_id => 0,
	id => 0,
    });
    if (defined $p{time} && !ref($p{time})) {
        $p{time} = Trasker::Date->new($p{time});
    }

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

#    map({bless $_, 'Trasker::TTDB::Note'} @{$sth->fetchall_arrayref({})});

    return $class->new($sth->fetchrow_hashref());
}

sub create
{
    my $class = shift;

    my $dbh = get_dbh;

    my $self = $class->new(@_);

    my $st;

    my $st_id = $dbh->prepare(qq/select nextval('notes_id_seq')/);

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
	$st->execute($self->type, $user_id, $project_id, $self->text, @extra);
    }; if ($@) {
	$dbh->rollback;
	die $@;
    } else {
	$dbh->commit or die $dbh->errstr();
    }
    $st->execute();

    use Data::Dumper;
    $st_id->execute();
    $self->{id} = $st_id->fetchrow();

    return $self;
}

sub id
{
    my $self = shift;

    $self->{id};
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
	$self->{user} ||= Trasker::TTDB::User->get(id => $self->user_id);
    }
}

sub project
{
    my $self = shift;

    require Trasker::TTDB::Project;
    if ($self->project_id) {
	$self->{project} ||= Trasker::TTDB::Project->get(id => $self->project_id);
    }
    $self->{project};
}

sub date
{
    my $self = shift;

    Trasker::Date->new($self->{time});
}

sub text
{
    my $self = shift;

    $self->{note};
}

1;
__END__

=head1 NAME

Trasker::TTDB::Note - Perl interface to the tasker notes table

=head1 SYNOPSIS

  use Trasker::TTDB::Note;

  $auto = Trasker::TTDB::Note->new(user => $user);

  $auto = Trasker::TTDB::Note->create(
      project_id => I<projectid>,
      user_id => I<userid>,
  );

  $auto = Trasker::TTDB::Note->get(user => $user, role => 'bob'):

=head1 DESCRIPTION

Normally the I<note> is returned from the I<Trasker::TTDB::Notes> interface or somewhere else.

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

=item text

Get the text for the note.

=item time

Get the time that the note was created.
The time is returned in a Date::OO Object.

=item type

This returns the type of the note.

=item  user_id

return the I<user_id> for the object.

=item user

return a I<user> Object for the note.

=item  project_id

return the I<project_id> for the object.

=item project

return a I<project> Object for the note.

=item delete

This will delete the object from the database.

=back

=head1 SEE ALSO

L<Trasker::TTDB::Notes>

=head1 AUTHOR

"G. Allen Morris III" <gam3@gam3.net>

=cut

