template <class T>
void bitpit::vtk::allocate( T & data, int n){
    return;
}

template <class T>
void bitpit::vtk::allocate( std::vector<T> & data, int n){
    data.resize(n);
    return;
}