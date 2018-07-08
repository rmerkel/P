/********************************************************************************************//**
 * @file streams.h
 *
 * class Stream<T>, a retargitable stream. InputStream and OutputStream.
 *
 * @author Randy Merkel, Slowly but Surly Software.
 * @copyright  (c) 2017 Slowly but Surly Software. All rights reserved.
 ************************************************************************************************/

#ifndef	STREAM_H
#define	STREAM_H

#include <istream>
#include <ostream>

/********************************************************************************************//**
 * Either a std::istream, or a std::ostream.
 *
 * Contains either a weak referance, such as std::cin, or a strong refernce such as a
 * std::ofstream.
 ************************************************************************************************/
template<class T> class Stream {
public:
	T*	stream;							///< Pointer to the stream

	/// Initialize with an stream which this does not own
	Stream(T& s) : stream{&s}, owns{false}	{}

	/// Initialize with an stream which this does own
	Stream(T* p) : stream{p}, owns{true}	{}

	/// Destructor
	virtual ~Stream()					{	close();	}

	void set_stream(T& s);				///< Set stream to s
	void set_stream(T* p);				///< Set stream to p

	/// If *this* owns stream, delete it.
	void close()						{	if (owns) delete stream;	}

private:
	bool			owns;				///< Does *this* own the stream?
};

/**
 * Set the input stream to a reference to s.
 * @param	s	The new input stream
 */
template<class T> void Stream<T>::set_stream(T& s) {
	close();
	stream = &s;
	owns = false;
}

/**
 * Set th input stream to p, taking ownership of that stream.
 * @param	p	The new input stream
 */
template<class T> void Stream<T>::set_stream(T* p) {
	close();
	stream = p;
	owns = true;
}

/// An input stream; std::cin or a std::ifstream
typedef Stream<std::istream>	InputStream;

/// An output stream; std::cout, std::cerr or an std::ofstream
typedef	Stream<std::ostream>	OutputStream;

#endif
