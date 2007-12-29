#ifndef __polkaIDL_hh__
#define __polkaIDL_hh__

#include <omniORB2/CORBA.h>

#include <polkaTypes.hh>
_CORBA_MODULE polka {
_CORBA_MODULE_PUBLIC

#ifndef __polka_loader__
#define __polka_loader__
  class   loader;
  typedef loader* loader_ptr;
  typedef loader_ptr loaderRef;

  class loader_Helper {
    public:
    static loader_ptr _nil();
    static CORBA::Boolean is_nil(loader_ptr p);
    static void release(loader_ptr p);
    static loader_ptr unmarshalObjRef(MemBufferedStream &s);
  };
  typedef _CORBA_ObjRef_Var<loader,loader_Helper> loader_var;

#endif
#define polka_loader_IntfRepoID "IDL:polka/loader:1.0"

  class _sk_loader :  public virtual loader {
  public:

    _sk_loader() {}
    _sk_loader(const omniORB::objectKey& k);
    virtual ~_sk_loader() {}
    loader_ptr _this() { return loader::_duplicate(this); }
    void _obj_is_ready(CORBA::BOA_ptr boa) { boa->obj_is_ready(this); }

  protected:
    virtual void *_widenFromTheMostDerivedIntf(const char *repoId) {
      return loader::_widenFromTheMostDerivedIntf(repoId);
    }
  private:
    _sk_loader (const _sk_loader&);
    _sk_loader &operator=(const _sk_loader&);
  };

  class _proxy_loader :  public virtual loader {
  private:

    _proxy_loader (const _proxy_loader&);
    _proxy_loader &operator=(const _proxy_loader&);
  };

  class _nil_loader :   public virtual loader {
  public:
    _nil_loader() { this->PR_setobj(0); }
    virtual ~_nil_loader() {}
    void dump (  ){
      throw CORBA::BAD_OPERATION(0,CORBA::COMPLETED_NO);
      // never reach here! Dummy return to keep some compilers happy.
      return;
    }

    polkaStatus  enregistreServeur ( const char *  name ){
      throw CORBA::BAD_OPERATION(0,CORBA::COMPLETED_NO);
      // never reach here! Dummy return to keep some compilers happy.
      polkaStatus _result = 0;
      return _result;
    }

    polkaStatus  enregistreClient ( const char *  name, CORBA::Long & numSerie ){
      throw CORBA::BAD_OPERATION(0,CORBA::COMPLETED_NO);
      // never reach here! Dummy return to keep some compilers happy.
      polkaStatus _result = 0;
      return _result;
    }

    polkaStatus  supprimeServeur ( const char *  name ){
      throw CORBA::BAD_OPERATION(0,CORBA::COMPLETED_NO);
      // never reach here! Dummy return to keep some compilers happy.
      polkaStatus _result = 0;
      return _result;
    }

    polkaStatus  supprimeClient ( const char *  name, CORBA::Long  numSerie ){
      throw CORBA::BAD_OPERATION(0,CORBA::COMPLETED_NO);
      // never reach here! Dummy return to keep some compilers happy.
      polkaStatus _result = 0;
      return _result;
    }

    void terminaisonClient (  ){
      throw CORBA::BAD_OPERATION(0,CORBA::COMPLETED_NO);
      // never reach here! Dummy return to keep some compilers happy.
      return;
    }

  protected:
    virtual void *_widenFromTheMostDerivedIntf(const char *repoId) {
      return loader::_widenFromTheMostDerivedIntf(repoId);
    }
  };

  class loader_proxyObjectFactory : public proxyObjectFactory {
  public:
    loader_proxyObjectFactory () {}
    virtual ~loader_proxyObjectFactory () {}
    virtual const char *irRepoId() const;
    virtual CORBA::Object_ptr newProxyObject(Rope *r,CORBA::Octet *key,size_t keysize,IOP::TaggedProfileList *profiles,CORBA::Boolean release);
    virtual CORBA::Boolean is_a(const char *base_repoId) const;
    static polka::loader_ptr _nil() {
      if (!__nil_loader) {
        __nil_loader = new polka::_nil_loader;
      }
      return __nil_loader;
    }
  private:
    static polka::loader_ptr __nil_loader;
  };
};

#endif // __polkaIDL_hh__
