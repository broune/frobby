/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
#ifndef NAME_FACTORY_GUARD
#define NAME_FACTORY_GUARD

// A NameFactory takes a name and then creates an instance of a class
// that has been previously registered under that name. This is done
// in a general way using templates.
//
// There are also some utility functions concerned with finding names
// from a prefix of that name.

#include <vector>
#include <string>

template<class AbstractProduct>
class NameFactory {
 public:
  // Returns null if name has not been registered. Otherwse calls the
  // function registered to that name and returns the result.
  auto_ptr<AbstractProduct> create(const string& name);

  typedef auto_ptr<AbstractProduct> (*FactoryFunction)();
  void registerProduct(const string& name, FactoryFunction function);

  // Returns null if no name is uniquely determined by the
  // prefix. Otherwse calls the function registered to that name and
  // returns the result.
  auto_ptr<AbstractProduct> createWithPrefix(const string& prefix);
  void addNamesWithPrefix(const string& prefix,
						  vector<string>& names);
  size_t countNamesWithPrefix(const string& prefix) const;

  bool isEmpty() const;

 private:
  class Pair {
  public:
	Pair(const string& name, FactoryFunction function):
	  _name(name),
	  _function(function) {
	  ASSERT(function != 0);
	}

	const string& getName() const {
	  return _name;
	}

	bool hasPrefix(const string& prefix) const {
	  return _name.compare(0, prefix.size(), prefix) == 0;
	}

	auto_ptr<AbstractProduct> create() const {
	  ASSERT(_function != 0);
	  return _function();
	}

  private:
	string _name;
	FactoryFunction _function;
  };

  vector<Pair> _pairs;
};

// This is a utility function wrapping the registerProduct method of a
// NameFactory. It would make more sense as a member function, but
// some compilers have problems with template member functions.
//
// The ConcreteProduct class must have a static method staticGetName
// which returns a std::string or a const char*.
template<class ConcreteProduct, class AbstractProduct>
void nameFactoryRegister(NameFactory<AbstractProduct>& factory);



// These are implementations that have to be included here due to
// being templates.

template<class AbstractProduct>
auto_ptr<AbstractProduct> NameFactory<AbstractProduct>::
create(const string& name) {
  for (typename vector<Pair>::const_iterator it = _pairs.begin();
	   it != _pairs.end(); ++it)
	if (it->getName() == name)
	  return it->create();
  return auto_ptr<AbstractProduct>();
}

template<class AbstractProduct>
auto_ptr<AbstractProduct> NameFactory<AbstractProduct>::
createWithPrefix(const string& prefix) {
  typename vector<Pair>::const_iterator match = _pairs.end();
  for (typename vector<Pair>::const_iterator it = _pairs.begin();
	   it != _pairs.end(); ++it) {
	if (it->hasPrefix(prefix)) {
	  if (match != _pairs.end())
		return auto_ptr<AbstractProduct>(); // not unique
	  match = it; // first match
	}
  }

  if (match == _pairs.end())
	return auto_ptr<AbstractProduct>();
  else
	return match->create();
}

template<class AbstractProduct>
void NameFactory<AbstractProduct>::
registerProduct(const string& name, FactoryFunction function) {
  _pairs.push_back(Pair(name, function));
}

template<class AbstractProduct>
void NameFactory<AbstractProduct>::
addNamesWithPrefix(const string& prefix, vector<string>& names) {
  for (typename vector<Pair>::const_iterator it = _pairs.begin();
	   it != _pairs.end(); ++it)
	if (it->hasPrefix(prefix))
	  names.push_back(it->getName());
}

template<class AbstractProduct>
size_t NameFactory<AbstractProduct>::
countNamesWithPrefix(const string& prefix) const {
  size_t count = 0;
  for (typename vector<Pair>::const_iterator it = _pairs.begin();
	   it != _pairs.end(); ++it)
	if (it->hasPrefix(prefix))
	  ++count;
  return count;
}

template<class AbstractProduct>
bool NameFactory<AbstractProduct>::isEmpty() const {
  return _pairs.empty();
}

namespace {
  // Helper function for nameFactoryRegister.
  template<class AbstractProduct, class ConcreteProduct>
  auto_ptr<AbstractProduct> createConcreteProductHelper() {
	return auto_ptr<AbstractProduct>(new ConcreteProduct());
  }
}

template<class ConcreteProduct, class AbstractProduct>
  void nameFactoryRegister(NameFactory<AbstractProduct>& factory) {
  
  const char* name = ConcreteProduct::staticGetName();
  typename NameFactory<AbstractProduct>::FactoryFunction
	createConcreteProduct =
	createConcreteProductHelper<AbstractProduct, ConcreteProduct>;
  
  factory.registerProduct(name, createConcreteProduct);
}

#endif
