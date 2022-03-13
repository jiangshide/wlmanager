package com.jingos;

interface IBinderWLManagerService {
    void StartApp(String package_name);
    void StopApp(String package_name);
    void DestroyApp(String package_name);

    void Ping(int i);
}
