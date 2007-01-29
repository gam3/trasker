--
-- PostgreSQL database dump
--

SET client_encoding = 'UNICODE';
SET check_function_bodies = false;

SET SESSION AUTHORIZATION 'gam3';

SET search_path = public, pg_catalog;

--
-- TOC entry 27 (OID 212354)
-- Name: plpgsql_call_handler(); Type: FUNC PROCEDURAL LANGUAGE; Schema: public; Owner: gam3
--

CREATE FUNCTION plpgsql_call_handler() RETURNS language_handler
    AS '$libdir/plpgsql', 'plpgsql_call_handler'
    LANGUAGE c;


SET SESSION AUTHORIZATION DEFAULT;

--
-- TOC entry 26 (OID 212355)
-- Name: plpgsql; Type: PROCEDURAL LANGUAGE; Schema: public; Owner: 
--

CREATE TRUSTED PROCEDURAL LANGUAGE plpgsql HANDLER plpgsql_call_handler;


SET SESSION AUTHORIZATION 'postgres';

--
-- TOC entry 4 (OID 2200)
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
GRANT ALL ON SCHEMA public TO PUBLIC;


SET SESSION AUTHORIZATION 'gam3';

--
-- TOC entry 5 (OID 26068)
-- Name: auto_id_seq; Type: SEQUENCE; Schema: public; Owner: gam3
--

CREATE SEQUENCE auto_id_seq
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 6 (OID 26076)
-- Name: notes_id_seq; Type: SEQUENCE; Schema: public; Owner: gam3
--

CREATE SEQUENCE notes_id_seq
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 7 (OID 26088)
-- Name: project_id_seq; Type: SEQUENCE; Schema: public; Owner: gam3
--

CREATE SEQUENCE project_id_seq
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 10 (OID 26090)
-- Name: project_hide_constraint_table; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE project_hide_constraint_table (
    hide character varying(3) NOT NULL
);


--
-- TOC entry 8 (OID 26108)
-- Name: timeslice_id_seq; Type: SEQUENCE; Schema: public; Owner: gam3
--

CREATE SEQUENCE timeslice_id_seq
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 11 (OID 26110)
-- Name: timeslice_temporary_constraint_table; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE timeslice_temporary_constraint_table (
    "temporary" character varying(9) NOT NULL
);


--
-- TOC entry 12 (OID 96421)
-- Name: auto; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE auto (
    id integer DEFAULT nextval('auto_id_seq'::text) NOT NULL,
    project_id integer,
    user_id integer,
    host character varying(40),
    name character varying(30),
    "class" character varying(30),
    role character varying(30) DEFAULT '%'::character varying,
    title character varying(100),
    desktop character varying(30),
    presidence integer
);


--
-- TOC entry 13 (OID 96444)
-- Name: notes; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE notes (
    id integer DEFAULT nextval('notes_id_seq'::text) NOT NULL,
    "time" timestamp without time zone,
    "type" integer DEFAULT 1,
    user_id integer DEFAULT 0 NOT NULL,
    project_id integer,
    note text
);


--
-- TOC entry 14 (OID 107044)
-- Name: project; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE project (
    id integer DEFAULT nextval('project_id_seq'::text) NOT NULL,
    parent_id integer,
    name character varying(30),
    user_id integer,
    hide character varying,
    description text
);


--
-- TOC entry 15 (OID 107097)
-- Name: timeslice; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE timeslice (
    id integer DEFAULT nextval('timeslice_id_seq'::text) NOT NULL,
    user_id integer,
    project_id integer,
    start_time timestamp without time zone,
    end_time timestamp without time zone,
    "temporary" character varying,
    auto_id integer DEFAULT 0,
    revert_to integer,
    host character varying(30),
    elapsed interval
);


--
-- TOC entry 16 (OID 118297)
-- Name: user_project; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE user_project (
    user_id integer,
    project_id integer
);


--
-- TOC entry 17 (OID 118334)
-- Name: users; Type: TABLE; Schema: public; Owner: gam3
--

CREATE TABLE users (
    id integer DEFAULT nextval('users_id_seq'::text) NOT NULL,
    name character varying(50),
    fullname character varying(50)
);


--
-- TOC entry 9 (OID 178291)
-- Name: users_id_seq; Type: SEQUENCE; Schema: public; Owner: gam3
--

CREATE SEQUENCE users_id_seq
    START WITH 5
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 28 (OID 212364)
-- Name: _group_concat(text, text); Type: FUNCTION; Schema: public; Owner: gam3
--

CREATE FUNCTION _group_concat(text, text) RETURNS text
    AS 'SELECT CASE WHEN $2 IS NULL THEN $1 WHEN $1 IS NULL THEN $2 ELSE $1 || '','' || $2 END;'
    LANGUAGE sql IMMUTABLE;


--
-- TOC entry 29 (OID 212365)
-- Name: group_concat(text); Type: AGGREGATE; Schema: public; Owner: gam3
--

CREATE AGGREGATE group_concat (
    BASETYPE = text,
    SFUNC = _group_concat,
    STYPE = text
);


--
-- TOC entry 22 (OID 212366)
-- Name: uk_notes_time_user; Type: INDEX; Schema: public; Owner: gam3
--

CREATE UNIQUE INDEX uk_notes_time_user ON notes USING btree ("time", note);


--
-- TOC entry 18 (OID 26092)
-- Name: project_hide_constraint_table_pkey; Type: CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY project_hide_constraint_table
    ADD CONSTRAINT project_hide_constraint_table_pkey PRIMARY KEY (hide);


--
-- TOC entry 19 (OID 26112)
-- Name: timeslice_temporary_constraint_table_pkey; Type: CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY timeslice_temporary_constraint_table
    ADD CONSTRAINT timeslice_temporary_constraint_table_pkey PRIMARY KEY ("temporary");


--
-- TOC entry 20 (OID 96425)
-- Name: auto_pkey; Type: CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY auto
    ADD CONSTRAINT auto_pkey PRIMARY KEY (id);


--
-- TOC entry 21 (OID 96452)
-- Name: notes_pkey; Type: CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY notes
    ADD CONSTRAINT notes_pkey PRIMARY KEY (id);


--
-- TOC entry 23 (OID 107050)
-- Name: project_pkey; Type: CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY project
    ADD CONSTRAINT project_pkey PRIMARY KEY (id);


--
-- TOC entry 24 (OID 107104)
-- Name: timeslice_pkey; Type: CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY timeslice
    ADD CONSTRAINT timeslice_pkey PRIMARY KEY (id);


--
-- TOC entry 25 (OID 118337)
-- Name: user_pkey; Type: CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY users
    ADD CONSTRAINT user_pkey PRIMARY KEY (id);


--
-- TOC entry 32 (OID 107052)
-- Name: project_hide_constraint; Type: FK CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY project
    ADD CONSTRAINT project_hide_constraint FOREIGN KEY (hide) REFERENCES project_hide_constraint_table(hide);


--
-- TOC entry 35 (OID 107106)
-- Name: timeslice_temporary_constraint; Type: FK CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY timeslice
    ADD CONSTRAINT timeslice_temporary_constraint FOREIGN KEY ("temporary") REFERENCES timeslice_temporary_constraint_table("temporary");


--
-- TOC entry 33 (OID 118343)
-- Name: fk_timeslice_user; Type: FK CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY timeslice
    ADD CONSTRAINT fk_timeslice_user FOREIGN KEY (user_id) REFERENCES users(id);


--
-- TOC entry 34 (OID 118347)
-- Name: fk_timeslice_project; Type: FK CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY timeslice
    ADD CONSTRAINT fk_timeslice_project FOREIGN KEY (project_id) REFERENCES project(id);


--
-- TOC entry 30 (OID 118351)
-- Name: fk_notes_project; Type: FK CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY notes
    ADD CONSTRAINT fk_notes_project FOREIGN KEY (project_id) REFERENCES project(id);


--
-- TOC entry 31 (OID 118355)
-- Name: fk_notes_user; Type: FK CONSTRAINT; Schema: public; Owner: gam3
--

ALTER TABLE ONLY notes
    ADD CONSTRAINT fk_notes_user FOREIGN KEY (user_id) REFERENCES users(id);


SET SESSION AUTHORIZATION 'postgres';

--
-- TOC entry 3 (OID 2200)
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON SCHEMA public IS 'Standard public schema';


