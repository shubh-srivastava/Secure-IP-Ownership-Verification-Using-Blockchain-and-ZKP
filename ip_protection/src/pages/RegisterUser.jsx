import { useState } from "react";

export default function RegisterUser() {
  const [userID, setUserID] = useState("");
  const [msg, setMsg] = useState("");

  async function submit(e) {
    e.preventDefault();

    const res = await fetch("http://localhost:18080/register", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ userID })
    });

    const data = await res.json();

    if (data.success) {
      setMsg("✅ User registered successfully");
      setUserID("");
    } else {
      setMsg("❌ User already exists");
    }
  }

  return (
    <div className="max-w-md mx-auto mt-10 bg-white p-6 rounded shadow">
      <h2 className="text-2xl font-bold mb-4">Register User</h2>

      <form onSubmit={submit} className="flex gap-3">
        <input
          className="flex-1 border p-2 rounded"
          value={userID}
          onChange={e => setUserID(e.target.value)}
          placeholder="User ID"
          required
        />
        <button className="bg-green-600 text-white px-4 rounded">
          Register
        </button>
      </form>

      {msg && <p className="mt-3">{msg}</p>}
    </div>
  );
}
