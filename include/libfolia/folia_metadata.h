/*
  Copyright (c) 2006 - 2022
  CLST  - Radboud University
  ILK   - Tilburg University

  This file is part of libfolia

  libfolia is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  libfolia is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.

  For questions and suggestions, see:
      https://github.com/LanguageMachines/ticcutils/issues
  or send mail to:
      lamasoftware (at ) science.ru.nl
*/

#ifndef FOLIA_METADATA_H
#define FOLIA_METADATA_H

namespace folia {

#define META_NOT_IMPLEMENTED {					       \
    throw NotImplementedError( "MetaTags::" + std::string(__func__) ); \
  }

  class FoliaElement;

  class MetaData {
  public:
  MetaData( const std::string& type ): _type(type){};
    virtual ~MetaData(){};
    virtual void add_av( const std::string&, const std::string& ) META_NOT_IMPLEMENTED;
    virtual const KWargs& get_avs() const META_NOT_IMPLEMENTED;
    virtual const std::string get_val( const std::string& ) const {
      return "";
    }
    virtual void add_foreign( const xmlNode * ) META_NOT_IMPLEMENTED;
    virtual std::string datatype() const { return "BaseMetaData"; };
    std::string type() const { return _type; };
    virtual std::string src() const META_NOT_IMPLEMENTED;
    virtual const std::vector<FoliaElement*> get_foreigners() const META_NOT_IMPLEMENTED;
  private:
    std::string _type;
  };

  class NativeMetaData: public MetaData {
  public:
    explicit NativeMetaData( const std::string& t ): MetaData(t) {};
    void add_av( const std::string& a, const std::string& v ) override
    { _attribs[a] = v; };
    const KWargs& get_avs() const override {
      return _attribs;
    }
    const std::string get_val( const std::string& at ) const override {
      auto const& it = _attribs.find( at );
      if ( it != _attribs.end() ){
	return it->second;
      }
      return "";
    }
    std::string datatype() const override { return "NativeMetaData"; };
  private:
    KWargs _attribs;
  };

  class ForeignMetaData: public MetaData {
  public:
  ForeignMetaData( const std::string& t ): MetaData(t) {};
    ~ForeignMetaData();
    void add_foreign( const xmlNode * ) override;
    std::string datatype() const override { return "ForeignMetaData"; };
    const std::vector<FoliaElement*> get_foreigners() const override {
      return foreigners;};
  private:
    std::vector<FoliaElement*> foreigners;
  };

  class ExternalMetaData: public MetaData {
  public:
  ExternalMetaData( const std::string& t,
		    const std::string& src ): MetaData(t) { _src = src; };
    std::string datatype() const override { return "ExternalMetaData"; };
    std::string src() const override { return _src; };
  private:
    std::string _src;
  };

} // end namespace

#endif // FOLIA_METADATA_H
