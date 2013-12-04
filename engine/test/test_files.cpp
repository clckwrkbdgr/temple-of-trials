#include "../files.h"
#include "../map.h"
#include "../test.h"
#include <sstream>

struct ComplexStructure {
	int i;
	char ch;
	ComplexStructure(int _i, char _ch) : i(_i), ch(_ch) {}
};

SAVEFILE_STORE_EXT(ComplexStructure, c)
{
	savefile.store(c.i);
	savefile.store(c.ch);
}

void store_ext(Writer & savefile, const int & i)
{
	savefile.store(i);
}

void store_ext(Reader & savefile, int & i)
{
	savefile.store(i);
}

SUITE(files) {

TEST(reader_should_skip_newline)
{
	std::istringstream in("\n");
	Reader reader(in);
	reader.newline();
	EQUAL(int(in.tellg()), 0);
}

TEST(reader_should_throw_exception_when_major_version_is_wrong)
{
	std::istringstream in("1 2 ");
	Reader reader(in);
	try {
		reader.version(2, 2);
	} catch(Reader::Exception & e) {
		EQUAL(
				e.message,
				"Savefile has major version 1, which is incompatible with current program savefile major version 2."
			 );
		return;
	}
	ASSERT(false);
}

TEST(reader_should_throw_exception_when_minor_version_is_less)
{
	std::istringstream in("1 2 ");
	Reader reader(in);
	try {
		reader.version(1, 1);
	} catch(Reader::Exception & e) {
		EQUAL(
				e.message,
				"Savefile has minor version 2, which is incompatible with current program savefile minor version 1."
			 );
		return;
	}
	ASSERT(false);
}

TEST(reader_should_throw_exception_when_stream_is_bad)
{
	std::istringstream in(" ");
	Reader reader(in);
	int i;
	reader.store(i);
	try {
		reader.check("test");
	} catch(Reader::Exception & e) {
		EQUAL(
				e.message,
				"Error: savefile is corrupted (reading test) ."
			 );
		return;
	}
	ASSERT(false);
}

TEST(reader_should_read_int)
{
	std::istringstream in("-1 ");
	Reader reader(in);
	int i;
	reader.store(i);
	EQUAL(i, -1);
}

TEST(reader_should_read_unsigned)
{
	std::istringstream in("1 ");
	Reader reader(in);
	unsigned int i;
	reader.store(i);
	EQUAL(i, (unsigned)1);
}

TEST(reader_should_read_char_as_int)
{
	std::istringstream in("65 ");
	Reader reader(in);
	char ch;
	reader.store(ch);
	EQUAL(ch, 'A');
}

TEST(reader_should_read_bool_as_int)
{
	std::istringstream in("1 ");
	Reader reader(in);
	bool b;
	reader.store(b);
	EQUAL(b, true);
}

TEST(reader_should_read_string_as_quoted_string)
{
	std::istringstream in("\"hello world\" ");
	Reader reader(in);
	std::string s;
	reader.store(s);
	EQUAL(s, "hello world");
}

TEST(reader_should_read_string_with_quote_in_it_escaped)
{
	std::istringstream in("\"hello \\\"world\\\"\" ");
	Reader reader(in);
	std::string s;
	reader.store(s);
	EQUAL(s, "hello \"world\"");
}

TEST(reader_should_read_size_of_map_and_resize_it)
{
	std::istringstream in("3 4 ");
	Reader reader(in);
	Map map(1, 1);
	reader.size_of(map);
	EQUAL(map.width, (unsigned)3);
	EQUAL(map.height, (unsigned)4);
}

TEST(reader_should_read_size_of_vector_and_resize_it)
{
	std::istringstream in("3 ");
	Reader reader(in);
	std::vector<int> v;
	reader.size_of(v);
	EQUAL(v.size(), (unsigned)3);
}

TEST(reader_should_resize_vector_and_read_it)
{
	std::istringstream in("3 1 2 3 ");
	Reader reader(in);
	std::vector<int> v;
	reader.store(v, "vector");
	EQUAL(v.size(), (unsigned)3);
	EQUAL(v[0], 1);
	EQUAL(v[1], 2);
	EQUAL(v[2], 3);
}


TEST(writer_should_write_newline)
{
	std::ostringstream out;
	Writer writer(out);
	writer.newline();
	EQUAL(out.str(), "\n");
}

TEST(writer_should_write_major_and_minor_version)
{
	std::ostringstream out;
	Writer writer(out);
	writer.version(1, 2);
	EQUAL(out.str(), "1 2 ");
}

TEST(writer_should_write_int_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(int(-1));
	EQUAL(out.str(), "-1 ");
}

TEST(writer_should_write_unsigned_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(unsigned(1));
	EQUAL(out.str(), "1 ");
}

TEST(writer_should_write_char_as_int_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store('A');
	EQUAL(out.str(), "65 ");
}

TEST(writer_should_write_bool_as_int_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(true);
	EQUAL(out.str(), "1 ");
}

TEST(writer_should_write_quoted_string_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(std::string("hello world"));
	EQUAL(out.str(), "\"hello world\" ");
}

TEST(writer_should_write_string_with_quote_in_it_escaped)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(std::string("hello \"world\""));
	EQUAL(out.str(), "\"hello \\\"world\\\"\" ");
}


TEST(writer_should_write_size_of_map)
{
	std::ostringstream out;
	Writer writer(out);
	writer.size_of(Map(3, 4));
	EQUAL(out.str(), "3 4 ");
}

TEST(writer_should_write_size_of_vector)
{
	std::ostringstream out;
	Writer writer(out);
	writer.size_of(std::vector<int>(3));
	EQUAL(out.str(), "3 ");
}

TEST(writer_should_write_vector_preceeded_by_size)
{
	std::ostringstream out;
	Writer writer(out);
	int a[] = {1, 2, 3};
	writer.store(std::vector<int>(a, a + 3), "vector");
	EQUAL(out.str(), "3 \n1 \n2 \n3 \n");
}


TEST(should_read_complex_type_by_store_ext)
{
	std::istringstream in("-1 65 ");
	Reader reader(in);
	ComplexStructure c(0, 0);
	store_ext(reader, c);
	EQUAL(c.i, -1);
	EQUAL(c.ch, 'A');
}

TEST(should_write_complex_type_by_store_ext)
{
	std::ostringstream out;
	Writer writer(out);
	store_ext(writer, ComplexStructure(1, 'A'));
	EQUAL(out.str(), "1 65 ");
}

}
