using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StatusText : MonoBehaviour
{
    public Transform cameraTransform;
    public float distance;
    void Update()
    {
        var cameraLookAt = cameraTransform.localRotation * Vector3.forward;
        var angle = Mathf.Atan2(cameraLookAt.y, Mathf.Sqrt(1 - cameraLookAt.y * cameraLookAt.y)) * Mathf.Rad2Deg;

        var rotation = Quaternion.AngleAxis(angle - 90.0f, Vector3.right);
        var translation = new Vector3(0.0f, 0.0f, distance);

        transform.localPosition = (rotation * translation);
        transform.localRotation = rotation;

        print(angle);
    }
}
