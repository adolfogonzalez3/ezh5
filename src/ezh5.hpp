#ifndef EZH5_H
#define EZH5_H

#include <hdf5.h>
#include <hdf5_hl.h>

#ifndef CANNOT_FIND_BOOST
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#endif

#include <complex>
#include <string>
#include <vector>



namespace ezh5{

    using namespace std;
    
    template<typename T>
    struct TypeMem{
	static hid_t id;
    };
    
    // define complex number datatype layout
    template<typename T>
    struct TypeMem<std::complex<T> >{
	static hid_t id;
	TypeMem(){
	    H5Tinsert(id, "r", 0, TypeMem<T>::id);  // the name 'r' is to be compatible with h5py
	    H5Tinsert(id, "i", sizeof(T), TypeMem<T>::id); // TODO: use HOFFSET
	}
    };

    // template<>
    // struct TypeMem<const char*>{
    // 	static hid_t id;
    // 	TypeMem(){
    // 	    H5Tset_size(id, H5T_VARIABLE);
    // 	}
    // };  // because the TypeFile<const char*> need to be H5T_FORTRAN_S1, so I use specialization in operator=() instead

    
    /// char* dsname
    template<typename T>
    hid_t write(hid_t loc_id, const char* dsname, const T& buf){
	hid_t dataspace_id = H5Screate(H5S_SCALAR);
	hid_t dataset_id = H5Dcreate(loc_id, dsname, TypeMem<T>::id, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	hid_t error_id = H5Dwrite(dataset_id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &buf);
	H5Dclose(dataset_id);
	H5Sclose(dataspace_id);
	return error_id;
    }
    /// string dsname
    template<typename T>
    hid_t write(hid_t loc_id, const std::string& dsname, const T& buf){
	return write(loc_id, dsname.c_str(), buf);
    }

    // /// char* dsname     // TODO: use enable_if to distinguish this from above
    // template<typename VEC>
    // hid_t write(hid_t loc_id, const char* dsname, const VEC& vec){
    // 	hsize_t dims[1];
    // 	dims[0] = vec.size();
    // 	hid_t dp_id = H5Screate_simple(1, dims, NULL);
    // 	hid_t ds_id = H5Dcreate(loc_id, dsname, TypeMem<VEC::value_type>::id, dp_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    // 	hid_t err_id = H5Dwrite(ds_id, TypeMem<VEC::value_type>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vec[0]);
    // 	H5Dclose(ds_id);
    // 	H5Sclose(dp_id);
    // 	return err_id;
    // }
    // /// string dsname
    // template<typename VEC>
    // hid_t write(hid_t loc_id, const std::string& dsname, const VEC& vec){
    // 	return write(loc_id, dsname.c_str(), vec);
    // }


    /// write std::vector to hdf5
    /// char* dsname
    template<typename T>
    hid_t write(hid_t loc_id, const char* dsname, const std::vector<T>& vec){
	hsize_t dims[1];
	dims[0] = vec.size();
	//std::cout<<dsname<<std::endl;
	hid_t dp_id = H5Screate_simple(1, dims, NULL);
	assert(dp_id>=0);
	hid_t ds_id = H5Dcreate(loc_id, dsname, TypeMem<T>::id, dp_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	assert(ds_id>=0);
	hid_t err_id = H5Dwrite(ds_id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vec[0]);
	H5Dclose(ds_id);
	H5Sclose(dp_id);
	return err_id;
    }
    /// string dsname
    template<typename T>
    hid_t write(hid_t loc_id, const std::string& dsname, const std::vector<T>& vec){
	return write(loc_id, dsname.c_str(), vec);
    }
    
#ifdef _BOOST_UBLAS_VECTOR_
    /// char* dsname
    template<typename T>
    hid_t write(hid_t loc_id, const char* dsname, const boost::numeric::ublas::vector<T>& vec){
	hsize_t dims[1];
	dims[0] = vec.size();
	hid_t dp_id = H5Screate_simple(1, dims, NULL);
	hid_t ds_id = H5Dcreate(loc_id, dsname, TypeMem<T>::id, dp_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	hid_t err_id = H5Dwrite(ds_id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vec(0));
	H5Dclose(ds_id);
	H5Sclose(dp_id);
	return err_id;
    }

    /// string dsname
    template<typename T>
    hid_t write(hid_t loc_id, const std::string& dsname, const boost::numeric::ublas::vector<T>& vec){
	return write(loc_id, dsname.c_str(), vec);
    }
#endif
    

#ifdef _BOOST_UBLAS_MATRIX_
    template<typename T>
    hid_t write(hid_t loc_id, const char* dsname, const boost::numeric::ublas::matrix<T>& mat){
	hsize_t dims[2];
	dims[0] = mat.size1();
	dims[1] = mat.size2();
	hid_t dp_id = H5Screate_simple(2, dims, NULL);
	htri_t is_exist = H5Lexists(loc_id, dsname, H5P_DEFAULT);
	hid_t ds_id = -1;
	switch (is_exist){
	case true:
	    ds_id = H5Dopen1(loc_id, dsname);
	    break;
	case false:
	    ds_id = H5Dcreate(loc_id, dsname, TypeMem<T>::id, dp_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	    break;
	default:
	    break;
	}
	assert(ds_id >=0);
	hid_t err_id = H5Dwrite(ds_id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &mat(0,0));
	assert(err_id>=0);
	H5Dclose(ds_id);
	H5Sclose(dp_id);
	return err_id;
    }
    /// string dsname
    template<typename T>
    hid_t write(hid_t loc_id, const std::string& dsname, const boost::numeric::ublas::matrix<T>& mat){
	return write(loc_id, dsname.c_str(), mat);
    }
#endif



//---------- read function    
    template<typename T>
    hid_t read(hid_t loc_id, const char* dsname, T* p_buf){
	hid_t dataset_id = H5Dopen2(loc_id, dsname, H5P_DEFAULT);
	hid_t datatype_id = H5Dget_type(dataset_id);
	hid_t error_id = H5Dread(dataset_id, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, p_buf);
	H5Dclose(dataset_id);
	return error_id;
    }
    
    
    // hid_t write_complex_vec(hid_t loc_id, const char* name, const std::size_t vec_size, const std::complex<double>* buf){
    // 	hsize_t dim=vec_size;
    // 	hid_t dataspace_id = H5Screate_simple(1, &dim, NULL);
    // 	hid_t dataset_id = H5Dcreate(loc_id, name, complex_tid, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    // 	hid_t error_id = H5Dwrite(dataset_id, complex_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
    // 	H5Dclose(dataset_id);
    // 	H5Sclose(dataspace_id);
    // 	return error_id;
    // }

    // hid_t read_complex(hid_t loc_id, const char* name, std::complex<double>* buf){
    // 	hid_t dataset_id = H5Dopen2(loc_id, name, H5P_DEFAULT);
    // 	hid_t datatype_id = H5Dget_type(dataset_id);
    // 	hid_t error_id = H5Dread(dataset_id, datatype_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
    // 	H5Dclose(dataset_id);
    // 	return error_id;
    // }
    
}


namespace ezh5{

    template<> hid_t TypeMem<double>::id = H5T_NATIVE_DOUBLE;
    template<> hid_t TypeMem<float>::id = H5T_NATIVE_FLOAT;
    template<> hid_t TypeMem<int>::id = H5T_NATIVE_INT;
    template<> hid_t TypeMem<long>::id = H5T_NATIVE_LONG;
    template<> hid_t TypeMem<unsigned int>::id = H5T_NATIVE_UINT;
    template<> hid_t TypeMem<unsigned long>::id = H5T_NATIVE_ULONG;

    // hid_t TypeMem<const char*>::id =  H5Tcopy(H5T_C_S1);
    
    template<> hid_t TypeMem<std::complex<float> >::id = H5Tcreate(H5T_COMPOUND, sizeof(std::complex<float>));  // create compound datatype
    template<> hid_t TypeMem<std::complex<double> >::id = H5Tcreate(H5T_COMPOUND, sizeof(std::complex<double>));
    // TODO: why I need specification, instead of defining follow
    //template<typename T>  
    //hid_t TypeMem<std::complex<T> >::id = H5Tcreate(H5T_COMPOUND, sizeof(std::complex<T>));


    TypeMem<std::complex<float> > obj_to_run_constructor_float;  // to add LAYOUT (STRUCTURE) to compound datatype
    TypeMem<std::complex<double> > obj_to_run_constructor_double;
    // TypeMem<char*> obj_to_run_constructor_charp;

}

namespace ezh5{

    class ID {
    public:
	hid_t id;
	ID(): id(-1){}
	ID(hid_t id_in) : id(id_in){}

	~ID(){}
    
    };


    class Node :public ID{
    public:

	Node(){}
    
	Node(hid_t pid_in, const string& path_in)
	    : ID(-1),
	      pid(pid_in),
	      path(path_in){
	    //cout<<"creating "<<path<<endl;
	}

    
	Node& operator()(const string& path){
	    return *this;
	}

    
	Node operator[](const string& path_more){
	    if(this->id==-1){// in lazy
		htri_t is_exist = H5Lexists(pid, path.c_str(), H5P_DEFAULT);
		if (is_exist<0){
		    assert(false);
		}else if (is_exist==false){
		    this->id = H5Gcreate2(pid, path.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
		}else{
		    this->id = H5Gopen(pid, path.c_str(), H5P_DEFAULT);
		}
	    }
	    assert(this->id>0);
	    return Node(this->id, path_more);
       	}


	/// write scalar
	template<typename T>
	Node& operator=(T val){
	    hid_t dataspace_id = -1;
	    if(this->id == -1){
		dataspace_id = H5Screate(H5S_SCALAR);
		this->id = H5Dcreate(pid, path.c_str(), TypeMem<T>::id, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	    }
	    hid_t error_id = H5Dwrite(id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &val);
	    H5Dclose(this->id);
	    this->id = -1;   // TODO: why keep on setting this->id here, not necessary
	    if (dataspace_id != -1) {H5Sclose(dataspace_id);}
	    return *this;
	}
	
	/// write std::vector
	template<typename T>
	Node& operator=(const std::vector<T>& vec){
	    hid_t dataspace_id = -1;
	    if(this->id == -1){
		hsize_t dims[1];
		dims[0] = vec.size();
		dataspace_id = H5Screate_simple(1, dims, NULL);
		assert(dataspace_id>=0);
		this->id = H5Dcreate(pid, path.c_str(), TypeMem<T>::id, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	    }
	    hid_t error_id = H5Dwrite(id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vec[0]);
	    H5Dclose(this->id);
	    this->id = -1;
	    if (dataspace_id != -1) {H5Sclose(dataspace_id);}
	    return *this;
	}

	/// write boost::numeric::ublas::vector
	#ifdef _BOOST_UBLAS_VECTOR_
	template<typename T>
	Node& operator=(const boost::numeric::ublas::vector<T>& vec){
	    hid_t dataspace_id = -1;
	    if(this->id == -1){
		hsize_t dims[1];
		dims[0] = vec.size();
		dataspace_id = H5Screate_simple(1, dims, NULL);
		assert(dataspace_id>=0);
		this->id = H5Dcreate(pid, path.c_str(), TypeMem<T>::id, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	    }
	    hid_t error_id = H5Dwrite(this->id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &vec(0));
	    H5Dclose(this->id);
	    this->id = -1;
	    if (dataspace_id != -1) {H5Sclose(dataspace_id);}
	    return *this;
	}
	#endif

	/// write boost::numeric::ublas::vector
	#ifdef _BOOST_UBLAS_MATRIX_
	template<typename T>
	Node& operator=(const boost::numeric::ublas::matrix<T>& mat){
	    hid_t dataspace_id = -1;
	    if(this->id == -1){
		hsize_t dims[2];
		dims[0] = mat.size1();
		dims[1] = mat.size2();
		hid_t dataspace_id = H5Screate_simple(2, dims, NULL);
		assert(dataspace_id>=0);
		this->id = H5Dcreate(this->pid, path.c_str(), TypeMem<T>::id, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	    }
	    hid_t error_id = H5Dwrite(this->id, TypeMem<T>::id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &mat(0,0));
	    assert(error_id>=0);
	    H5Dclose(this->id);
	    this->id = -1;
	    if (dataspace_id != -1) {H5Sclose(dataspace_id);}
	    return *this;
	}
	#endif
	
	
	~Node(){
	    if(this->id>0){
		//cout<<"closing "<<path<<endl;
		H5Gclose(this->id);
		this->id = -1;
	    }
	}
    
    public:
	hid_t pid; // parent_id
	string path;
    };

    template<>
    Node& Node::operator=(const char* str){
    	hid_t type_in_file = H5Tcopy(H5T_FORTRAN_S1);
    	H5Tset_size(type_in_file, H5T_VARIABLE);
    	hid_t type_in_mem = H5Tcopy(H5T_C_S1);
    	H5Tset_size(type_in_mem, H5T_VARIABLE);

    	hid_t dataspace_id = -1;
    	if(this->id == -1){
    	    hsize_t dims[1] = {1};
    	    dataspace_id = H5Screate(H5S_SCALAR);
    	    this->id = H5Dcreate(pid, path.c_str(), type_in_mem, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    	}
    	hid_t error_id = H5Dwrite(id, type_in_mem, H5S_ALL, H5S_ALL, H5P_DEFAULT, &str);
    	H5Dclose(this->id);
    	this->id = -1;
    	if (dataspace_id != -1) {H5Sclose(dataspace_id);}
    	H5Tclose(type_in_file);
    	H5Tclose(type_in_mem);
    	return *this;
    }

    class Dataset : public Node{
    };


    class File : public Node{
    public:
	// TODO: open an opened file
	// TODO: implement open if exists, create if not
	File(const string& path, unsigned flags)
	    : __auto_close(true){
	    if(flags==H5F_ACC_RDWR || flags==H5F_ACC_RDONLY){
		this->id = H5Fopen(path.c_str(), flags, H5P_DEFAULT);
	    }else if (flags==H5F_ACC_TRUNC || flags==H5F_ACC_EXCL){
		this->id = H5Fcreate(path.c_str(), flags, H5P_DEFAULT, H5P_DEFAULT);
	    }else{
		assert(false && "unknow file access mode");
	    }
	}

	/// a special constructor to construct File from an exiting file id
	/// this constructor should only be used for low level code to avoid confusing
	File(hid_t fid)
	    : __auto_close(false){
	    this->id = fid;
	}

	~File(){
	    if (__auto_close) {
		H5Fclose(this->id);
	    }
	    this->id = -1;  // so that ~Node will not try to close it again
	}
    private:
	bool __auto_close;
    };


}


#endif
